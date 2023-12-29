#include <glm/gtx/transform.hpp>
#include "skeletal_model.h"
#include "skeletalrenderer.h"
#include "../assetmanager.h"

using namespace monkey::skeletal;

SkeletalModel::SkeletalModel(const py::kwargs& kwargs) {

    for (const auto& j : kwargs["joints"]) {
        auto dict = j.cast<pybind11::dict>();
        int id = m_jointInfos.size();
        auto jointId = j["id"].cast<std::string>();
        auto meshId = j["mesh"].cast<std::string>();
        auto parent = py_get_dict<std::string>(dict, "parent", "");
        std::string keyPoint;
        int parentId = -1;
        if (!parent.empty()) {
            auto sep = parent.find('@');
            assert(sep != std::string::npos);
            parentId = m_jointNameToId[parent.substr(0, sep)];
            keyPoint = parent.substr(sep+1);
        }
        m_jointInfos.emplace_back(id, parentId, jointId, glm::ivec3(id, parentId == -1 ? 0 : parentId, 0));
        //std::cout << "*** joint " << jointId << ", id: " << id << " , " << meshId << ", parent = " << parent << "\n";
        glm::vec2 attachPoint(0.f);
        if (parentId != -1) {
            m_jointInfos[parentId].children.push_back(id);
            attachPoint = m_models[parentId]->getKeyPoint(keyPoint);

        } else {
            m_root = id;
        }
        auto scale = py_get_dict<float>(dict, "scale", 1.f);
        auto z = py_get_dict<float>(dict, "z", 0.f);
        auto offset = py_get_dict<glm::vec2>(dict, "offset", glm::vec2(0.f));
        attachPoint += offset;
        JointTransform tr;
        tr.scale = glm::vec3(scale);
        tr.translation = glm::vec3(attachPoint, 0.0f);
        m_jointInfos[id].scale = scale;
        m_jointInfos[id].z = z;
        m_jointInfos[id].restTransform = tr;

        auto mesh = AssetManager::instance().getPolyMesh(meshId);
        m_models.push_back(mesh);
        m_jointInfos[id].mesh = mesh.get();
        m_jointNameToId.insert(std::make_pair(jointId, id));
    }

	std::unordered_map<int, JointTransform> p;
	m_invRestTransforms2 = std::vector<glm::mat4>(m_jointInfos.size());
	for (size_t i = 0; i < m_invRestTransforms2.size(); ++i) {
		m_invRestTransforms2[i] = glm::mat4(1.0f);
	}
	m_restTransforms2 = calculateCurrentPose(p);
	for (size_t i = 0; i < m_invRestTransforms2.size(); ++i) {
		m_restTransforms2[i][3][2] = m_jointInfos[i].z;
		m_invRestTransforms2[i] = glm::inverse(m_restTransforms2[i]);
		//m_invRestTransforms2[i][3][2] = -m_jointInfos[i].z;
	}

    /**************
     * read anims *
     **************/
    int ac = 0;
    if (kwargs.contains("animations")) {
        auto anims = kwargs["animations"].cast<pybind11::list>();
        for (const auto& anim : anims) {
            auto id = anim["id"].cast<std::string>();
            auto path = anim["path"].cast<std::string>();
            int cp = py_get_dict<int>(anim, "collide_point", -1);
            if (cp != -1) {
                _collidePointIndices[id] = cp;
            }

            auto animation = AssetManager::instance().getSkeletalAnimation(path);
            _animations[id] = animation;
//                attackBox = dictget<std::vector<int>>(animInfo, "attack_box", std::vector<int>());
//                m_animations[id] = anim;
//            }
//            shapeVector.insert(shapeVector.end(), attackBox.begin(), attackBox.end());
//            m_animShapes[id] = shapeVector;
            if (m_defaultAnimation.empty()) {
                m_defaultAnimation = id;
            }
        }
    }

    /**************************************
     * read offset and collision points
     *************************************/
    if (kwargs.contains("offset")) {
        for (const auto& offset : kwargs["offset"]) {
            auto id = offset.cast<std::string>() ;
            auto atindex = id.find('@');
            assert(atindex != std::string::npos);
            m_offsetPointIds.emplace_back(id.substr(0, atindex), id.substr(atindex+1));
        }
        computeOffset(m_offsetPoints, m_offsetPointIds);
    }
    if (kwargs.contains("collide")) {
        std::vector<std::pair<std::string, std::string>> collidePoints;
        for (const auto& offset : kwargs["collide"]) {
            auto id = offset.cast<std::string>() ;
            auto atindex = id.find('@');
            assert(atindex != std::string::npos);
            collidePoints.emplace_back(id.substr(0, atindex), id.substr(atindex+1));
        }
        computeOffset(m_colliderPoints, collidePoints);
    }
    if (kwargs.contains("size")) {
        auto size = py_get_dict<std::string>(kwargs, "size");
        auto atindex = size.find('@');
        auto id = m_jointNameToId.at(size.substr(0, atindex));
        if (m_jointInfos[id].mesh != nullptr) {
            _sizeXZ = m_jointInfos[id].mesh->getDimension(size.substr(atindex+1));

        }
    }
}

void SkeletalModel::computeOffset(std::vector<std::pair<int, glm::vec3>>& points, const std::vector<std::pair<std::string, std::string>>& ids) {
    points.clear();
    for (const auto& p : ids) {
        int jointId = m_jointNameToId.at(p.first);
        if (m_jointInfos[jointId].mesh == nullptr) {
            continue;
        }
        auto kp = m_jointInfos[jointId].mesh->getKeyPoint(p.second);
        auto mp = m_restTransforms2[jointId] * glm::vec4(kp.x, kp.y, 0.0f, 1.0f);
        points.emplace_back(jointId, glm::vec3(mp.x, mp.y, 0.0f));
    }
}


std::shared_ptr<Renderer> SkeletalModel::getRenderer(const pybind11::kwargs& args) {
    return std::make_shared<SkeletalRenderer>(args);
}


std::vector<glm::mat4> SkeletalModel::calculateCurrentPose(std::unordered_map<int, JointTransform>& pose) {
	std::vector<glm::mat4> result(m_jointInfos.size());
	std::list<std::pair<int, glm::mat4>> joints;
	joints.emplace_back(m_root, glm::mat4(1.0));
	while (!joints.empty()) {
		auto current = joints.front();
		joints.pop_front();
		const auto& joint = m_jointInfos[current.first];
		auto localTransform = joint.restTransform;
		// TODO apply local transform
		auto i = pose.find(current.first);
		if (i != pose.end()) {
			localTransform += i->second;
		}
		auto localMat = localTransform.getLocalTransform();
		auto modelMat = current.second * localMat * glm::scale(glm::vec3(joint.scale));
		result[current.first] = modelMat * m_invRestTransforms2[current.first];
		// TODO convert to global mat
		for (const auto &child : joint.children) {
			joints.emplace_back(child, modelMat);
		}
	}
	return result;

}

int SkeletalModel::getJointId(const std::string & id) {
    auto i = m_jointNameToId.find(id);
    if (i == m_jointNameToId.end()) {
        return -1;
    }
    return i->second;
}

SkeletalAnimation * SkeletalModel::getAnimation(const std::string &id) {
    auto it = _animations.find(id);
    if (it == _animations.end())
        return nullptr;
    return it->second.get();
}

std::shared_ptr<Shape> SkeletalModel::getShape (const std::string& anim) const {
    const auto& it = _shapes.find(anim);
    if (it == _shapes.end())
        return nullptr;
    return it->second;
}

std::shared_ptr<Shape> SkeletalModel::getShapeCast(const std::string &anim) const {
    const auto& it = _shapeCast.find(anim);
    if (it == _shapeCast.end())
        return nullptr;
    return it->second;

}

int SkeletalModel::getCollidePointIndex(const std::string &anim) const {
    auto it = _collidePointIndices.find(anim);
    if (it == _collidePointIndices.end()) {
        return -1;
    }
    return it->second;
}