#include "skeletal_collider.h"
#include "../pyhelper.h"

using namespace monkey::skeletal;

SkeletalCollider::SkeletalCollider(const pybind11::kwargs& args) : Collider(args), m_model(nullptr), _renderer(nullptr), _collisionEngine(nullptr) {
    _castMask = py_get_dict<int>(args, "cast_mask", 0);
    _castTag = py_get_dict<int>(args, "cast_tag", 0);
}

void SkeletalCollider::start() {
    _renderer = dynamic_cast<SkeletalRenderer*>(m_node->getComponent<Renderer>());
    auto& engine = Engine::instance();
    auto room = engine.getRoom();
    _collisionEngine = room->getRunner<ICollisionEngine>();
}

std::shared_ptr<Shape> SkeletalCollider::getShape() {
    std::string anim = _renderer->getAnimation();
    return m_model->getShape(anim);
}

void SkeletalCollider::update(double) {
    // check if current (anim, t) has a shape to cast
    auto animId = _renderer->getAnimation();
    auto time = _renderer->getAnimationTime();
    auto box = m_model->getShapeCast(animId);
    bool hit = false;
    if (box != nullptr) {
        auto t = m_node->getWorldMatrix();
        auto e = _collisionEngine->shapeCast(box.get(), t, _castMask, true);
        if (!e.empty()) {
            hit = true;
            // TODO m_engine->processCollisions(e, m_node, m_castTag);
        }
    }
    if (!hit) {
        // m_lastHit = nullptr;
    }
}

SkeletalColliderRenderer::SkeletalColliderRenderer(const pybind11::kwargs& args) :
    BatchRenderer<LineBatch>(args) {
}

void SkeletalColliderRenderer::start() {
    BatchRenderer<LineBatch>::start();
    _refRenderer = dynamic_cast<SkeletalRenderer*>(m_node->getParent()->getComponent<Renderer>());
    _model = dynamic_cast<SkeletalModel*>(_refRenderer->getModel());

    // skeletal collider has 2 boxes: one for body, one for cast shape. You need 24 lines
    for (int i=0; i < 24; ++i) {
        _primitiveIds.push_back(_batch->getPrimitiveId());
    }
    _offsetMain = _primitiveIds.front();
    _offsetCast = _offsetMain + 12;
}


std::type_index SkeletalColliderRenderer::getType() {
    return std::type_index(typeid(Renderer));
}

void SkeletalColliderRenderer::update(double dt) {
    // at every point, just show the box(es) relative to animation

    auto animation = _refRenderer->getAnimation();
    _batch->hideLines(_primitiveIds[0], _primitiveIds.size());
    auto currentShape = _model->getShape(animation);

    if (currentShape != nullptr) {
        auto mat = m_node->getWorldMatrix();
        auto bounds = _model->getShape(animation)->getBounds();
        auto size = glm::vec3(mat * glm::vec4(bounds.getSize(), 0.f));
        auto minWorld = glm::vec3(mat * glm::vec4(bounds.min, 1.f));
    }
}