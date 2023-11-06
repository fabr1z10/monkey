#include <glm/gtx/transform.hpp>
#include "skeletal.h"
#include "../components/skeletalrenderer.h"
#include "../assetmanager.h"

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