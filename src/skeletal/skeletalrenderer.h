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

        float getAnimationTime() const;

        void update(double) override;

        void draw(Shader *) override;

        std::type_index getType() override;

        void setModel(std::shared_ptr<Model>, const pybind11::kwargs &args) override;

        SkeletalModel* getModel();

        void setAnimation(const std::string&) override;

        glm::vec3 getSize() const;

        bool isComplete() const override;

        Bounds getShapeCast() const;
    private:
        void innerDraw(Shader*,int,int,int);
        std::unordered_map<int, JointTransform> interpolatePoses(
                KeyFrame* previousFrame, KeyFrame* nextFrame, float progression);
        SkeletalModel *_model;
        std::vector<glm::mat4> _bones;
        SkeletalAnimation *m_currentAnimation;
        float m_animationTime;
        bool m_complete;
        bool _castShadow;
        float _shadowAlpha;
        float _shadowScale;
        glm::vec3 _size;
        Bounds _shapeCast;
    };

    inline SkeletalModel * SkeletalRenderer::getModel() {
        return _model;
    }

    inline std::type_index SkeletalRenderer::getType() {
        return std::type_index(typeid(Renderer));
    }

    inline float SkeletalRenderer::getAnimationTime() const {
        return m_animationTime;
    }

    inline glm::vec3 SkeletalRenderer::getSize() const {
        return _size;
    }

    inline bool SkeletalRenderer::isComplete() const {
        return m_complete;
    }

    inline Bounds SkeletalRenderer::getShapeCast() const {
        return _shapeCast;
    }
}