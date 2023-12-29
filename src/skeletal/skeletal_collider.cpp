#include <iostream>
#include "skeletal_collider.h"
#include "../pyhelper.h"
#include "../shapes3d/aabb3d.h"

using namespace monkey::skeletal;

SkeletalCollider::SkeletalCollider(const pybind11::kwargs& args) : Collider(args), m_model(nullptr), _renderer(nullptr) {
    _castMask = py_get_dict<int>(args, "cast_mask", 0);
    _castTag = py_get_dict<int>(args, "cast_tag", 0);
}

void SkeletalCollider::start() {
    _renderer = dynamic_cast<SkeletalRenderer*>(m_node->getComponent<Renderer>());
    //auto& engine = Engine::instance();
    //auto room = engine.getRoom();
    //_collisionEngine = room->getRunner<ICollisionEngine>();
    Collider::start();
}

std::shared_ptr<Shape> SkeletalCollider::getShape() {
    std::string anim = _renderer->getAnimation();
    return m_model->getShape(anim);
}

void SkeletalCollider::update(double) {
    // check if current (anim, t) has a shape to cast
    auto bounds = _renderer->getShapeCast();
    if (!bounds.isVoid()) {
        auto t = m_node->getWorldMatrix();
        AABB3D aabb(bounds.min.x, bounds.max.x, bounds.min.y, bounds.max.y, bounds.min.z, bounds.max.z);
        auto castResult = m_engine->shapeCast(&aabb, t, _castMask, true);
        if (!castResult.empty()) {
            std::cout << "PROCESS COLLISION\n";
        }
    }
//    auto animId = _renderer->getAnimation();
//    auto time = _renderer->getAnimationTime();
//    auto box = m_model->getShapeCast(animId);
//    bool hit = false;
//    if (box != nullptr) {
//        auto t = m_node->getWorldMatrix();
//        auto e = _collisionEngine->shapeCast(box.get(), t, _castMask, true);
//        if (!e.empty()) {
//            hit = true;
//            // TODO m_engine->processCollisions(e, m_node, m_castTag);
//        }
//    }
//    if (!hit) {
//        // m_lastHit = nullptr;
//    }
}

SkeletalColliderRenderer::SkeletalColliderRenderer(const pybind11::kwargs& args) :
    BatchRenderer<LineBatch>(args) {
    _debugColor = glm::vec4(1.f, 1.f, 1.f, 0.2f);
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
    auto s = _refRenderer->getSize();
    auto m = m_node->getWorldMatrix();

    glm::vec3 d = m * glm::vec4(s, 0.f);
    d.x = fabs(d.x);
    d.y = fabs(d.y);
    d.z = fabs(d.z);



    glm::vec3 A = glm::vec3(m[3]) + glm::vec3(-d[0], 0.f, -d[2]);
    glm::vec3 B = A + glm::vec3(2*d[0], 0.f, 0.f);
    glm::vec3 C = B + glm::vec3(0.f, 0.f, 2*d[2]);
    glm::vec3 D = C + glm::vec3(-2*d[0], 0.f, 0.f);
    glm::vec3 Ah = A + glm::vec3(0.f, d[1], 0.f);
    glm::vec3 Bh = B + glm::vec3(0.f, d[1], 0.f);
    glm::vec3 Ch = C + glm::vec3(0.f, d[1], 0.f);
    glm::vec3 Dh = D + glm::vec3(0.f, d[1], 0.f);


    _batch->setLine(_offsetMain, A, B, _debugColor);
    _batch->setLine(_offsetMain+1, B, C, _debugColor);
    _batch->setLine(_offsetMain+2, C, D, _debugColor);
    _batch->setLine(_offsetMain+3, D, A, _debugColor);

    _batch->setLine(_offsetMain+4, Ah, Bh, _debugColor);
    _batch->setLine(_offsetMain+5, Bh, Ch, _debugColor);
    _batch->setLine(_offsetMain+6, Ch, Dh, _debugColor);
    _batch->setLine(_offsetMain+7, Dh, Ah, _debugColor);
    _batch->setLine(_offsetMain+8, A, Ah, _debugColor);
    _batch->setLine(_offsetMain+9, B, Bh, _debugColor);
    _batch->setLine(_offsetMain+10, C, Ch, _debugColor);
    _batch->setLine(_offsetMain+11, D, Dh, _debugColor);

    // at every point, just show the box(es) relative to animation

    // now check shapecast
    auto b = _refRenderer->getShapeCast();
    if (!b.isVoid()) {
        glm::vec3 E = m * glm::vec4(b.min, 1.f);
        glm::vec3 F = m * glm::vec4(b.max, 1.f);

        _batch->setLine(_offsetCast, glm::vec3(E.x, E.y, E.z), glm::vec3(F.x, E.y, E.z), glm::vec4(1.f));
        _batch->setLine(_offsetCast+1, glm::vec3(E.x, E.y, E.z), glm::vec3(E.x, F.y, E.z), glm::vec4(1.f));
        _batch->setLine(_offsetCast+2, glm::vec3(E.x, F.y, E.z), glm::vec3(F.x, F.y, E.z), glm::vec4(1.f));
        _batch->setLine(_offsetCast+3, glm::vec3(F.x, F.y, E.z), glm::vec3(F.x, E.y, E.z), glm::vec4(1.f));

        _batch->setLine(_offsetCast+4, glm::vec3(E.x, E.y, F.z), glm::vec3(F.x, E.y, F.z), glm::vec4(1.f));
        _batch->setLine(_offsetCast+5, glm::vec3(E.x, E.y, F.z), glm::vec3(E.x, F.y, F.z), glm::vec4(1.f));
        _batch->setLine(_offsetCast+6, glm::vec3(E.x, F.y, F.z), glm::vec3(F.x, F.y, F.z), glm::vec4(1.f));
        _batch->setLine(_offsetCast+7, glm::vec3(F.x, F.y, F.z), glm::vec3(F.x, E.y, F.z), glm::vec4(1.f));

        _batch->setLine(_offsetCast+8, glm::vec3(E.x, E.y, E.z), glm::vec3(E.x, E.y, F.z), glm::vec4(1.f));
        _batch->setLine(_offsetCast+9, glm::vec3(E.x, F.y, E.z), glm::vec3(E.x, F.y, F.z), glm::vec4(1.f));
        _batch->setLine(_offsetCast+10, glm::vec3(F.x, E.y, E.z), glm::vec3(F.x, E.y, F.z), glm::vec4(1.f));
        _batch->setLine(_offsetCast+11, glm::vec3(F.x, F.y, E.z), glm::vec3(F.x, F.y, F.z), glm::vec4(1.f));
    }


//
//    auto animation = _refRenderer->getAnimation();
//    _batch->hideLines(_primitiveIds[0], _primitiveIds.size());
//    auto currentShape = _model->getShape(animation);
//
//    if (currentShape != nullptr) {
//        auto mat = m_node->getWorldMatrix();
//        auto bounds = _model->getShape(animation)->getBounds();
//        auto size = glm::vec3(mat * glm::vec4(bounds.getSize(), 0.f));
//        auto minWorld = glm::vec3(mat * glm::vec4(bounds.min, 1.f));
//    }
}

void SkeletalCollider::generateDebugMesh() {
    if (m_debugNode != nullptr) {
        m_debugNode->remove();
    }
//	auto model = m_sprite->generateDebugModel();
//
    if (_batchId.empty()) {
        return;
    }
    auto node = std::make_shared<Node>();
//	node->setNodeModel(model);
    pybind11::kwargs args;
    args["batch"] = _batchId;
    auto renderer = std::make_shared<SkeletalColliderRenderer>(args);

    //renderer->setNodeModel(model);
    node->addComponent(renderer);
    m_node->add(node);
    m_debugNode = node.get();
}