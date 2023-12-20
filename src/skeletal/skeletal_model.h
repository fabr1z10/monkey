#pragma once

#include "../model.h"
#include "../pyhelper.h"
#include "../components/renderer.h"
#include "joint_transform.h"
#include "animation.h"
#include "../models/polymesh.h"
#include "../shape.h"

namespace monkey::skeletal {

    struct JointInfo {
        JointInfo(int id, int parent, const std::string &name, glm::ivec3 weightIndex) : id(id),
                                                                                         parent(parent), name(name),
                                                                                         mesh(nullptr),
                                                                                         weightIndex(weightIndex),
                                                                                         scale(1.0f), z(0.0f) {}

        int id;
        int parent;
        float scale;
        float z;
        std::vector<int> children;
        std::string name;
        JointTransform restTransform;
        PolyMesh *mesh;
        glm::ivec3 weightIndex;
    };

    class SkeletalModel : public Model {
    public:
        explicit SkeletalModel(const py::kwargs &kwargs);

        std::shared_ptr<Renderer> getRenderer(const pybind11::kwargs &) override;

        const std::vector<std::shared_ptr<PolyMesh>> &getModels() const;

        const glm::mat4 &getRestTransform(int id) const;

        const JointInfo &getJointInfo(int id) const;

        std::vector<glm::mat4> calculateCurrentPose(std::unordered_map<int, JointTransform> &pose);

        int getJointId(const std::string&);

        const std::vector<std::pair<int, glm::vec3>>& getOffsetPoints() const;

        SkeletalAnimation* getAnimation(const std::string& id);

        std::shared_ptr<Shape> getShape (const std::string& anim) const;

        std::shared_ptr<Shape> getShapeCast (const std::string& anim) const;

        std::vector<std::string> getAnimationList() const;

        int getShapeCount() const;
    private:
        void computeOffset();
        std::string m_defaultAnimation;
        std::vector<JointInfo> m_jointInfos;
        std::unordered_map<std::string, int> m_jointNameToId;
        std::vector<std::shared_ptr<PolyMesh>> m_models;
        std::vector<glm::mat4> m_restTransforms2;
        std::vector<glm::mat4> m_invRestTransforms2;
        std::vector<std::pair<int, glm::vec3>> m_offsetPoints;
        std::unordered_map<std::string, std::shared_ptr<SkeletalAnimation>> _animations;
        std::vector<std::pair<std::string, std::string>> m_offsetPointIds;
        std::unordered_map<std::string, std::shared_ptr<Shape>> _shapes;
        std::unordered_map<std::string, std::shared_ptr<Shape>> _shapeCast;
        int m_root;
    };

    inline const std::vector<std::shared_ptr<PolyMesh>> &SkeletalModel::getModels() const {
        return m_models;
    }

    inline const glm::mat4 &SkeletalModel::getRestTransform(int id) const {
        return m_restTransforms2[id];
    }


    inline const JointInfo &SkeletalModel::getJointInfo(int id) const {
        return m_jointInfos[id];
    }

    inline const std::vector<std::pair<int, glm::vec3>>& SkeletalModel::getOffsetPoints() const {
        return m_offsetPoints;
    }
}