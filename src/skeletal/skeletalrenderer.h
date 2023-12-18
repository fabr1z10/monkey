#pragma once

#include <string>
#include "../shader.h"
#include "skeletal_model.h"
#include "animation.h"
#include "../components/batchrenderer.h"

namespace monkey::skeletal {
    class SkeletalRenderer : public BatchRenderer<ProvaBatch> {
    public:
        SkeletalRenderer(const pybind11::kwargs &args);

        ~SkeletalRenderer();

        void update(double) override;

        void draw(Shader *) override;

        std::type_index getType() override;

        void setModel(std::shared_ptr<Model>, const pybind11::kwargs &args) override;

        void setAnimation(const std::string&) override;
    private:
        std::unordered_map<int, JointTransform> interpolatePoses(
                KeyFrame* previousFrame, KeyFrame* nextFrame, float progression);
        SkeletalModel *_model;
        std::vector<glm::mat4> _bones;
        SkeletalAnimation *m_currentAnimation;
        float m_animationTime;
        bool m_complete;
    };

    inline std::type_index SkeletalRenderer::getType() {
        return std::type_index(typeid(Renderer));
    }
}