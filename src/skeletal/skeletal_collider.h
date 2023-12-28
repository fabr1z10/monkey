#pragma once

#include "../components/collider.h"
#include "skeletalrenderer.h"
#include "skeletal_model.h"
#include "../runners/collision_engine.h"

namespace monkey::skeletal {
    class SkeletalCollider : public Collider {
    public:
        explicit SkeletalCollider(const pybind11::kwargs &args);

        std::shared_ptr<Shape> getShape() override;

        void start() override;

        void update(double) override;

    private:
        int _castMask;
        int _castTag;
        SkeletalRenderer *_renderer;
        SkeletalModel *m_model;
        //ICollisionEngine* _collisionEngine;
        void generateDebugMesh() override;
    };

    class SkeletalColliderRenderer : public BatchRenderer<LineBatch> {
    public:
        SkeletalColliderRenderer(const pybind11::kwargs& args);
        //void draw(Shader * s) override;
        void start() override;
        std::type_index getType() override;
        void setModel(std::shared_ptr<Model>, const pybind11::kwargs& args) override {}
        void update(double) override;

    private:
        SkeletalRenderer* _refRenderer;
        SkeletalModel* _model;
        std::string _batchId;
        int _offsetMain;
        int _offsetCast;


    };

}
