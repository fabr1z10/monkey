#include "collider.h"
#include "../engine.h"
#include "../error.h"
#include "../runners/collision_engine.h"
#include "../pyhelper.h"
#include "../node.h"
#include "../models/modelmake.h"

Collider::Collider(int flag, int mask, int tag) : m_callbackHandle(-1), m_engine(nullptr), m_debugNode(nullptr), m_flag(flag), m_mask(mask), m_tag(tag) {

}

Collider::~Collider() {
    if (m_engine != nullptr)
        m_engine->remove(this);
    if (m_callbackHandle != -1)
        m_node->onMove.unreg(m_callbackHandle);
}

void Collider::setCollisionFlag(int flag) {
    m_flag = flag;
}

void Collider::setCollisionMask(int mask) {
    m_mask = mask;
}

void Collider::setCollisionTag(int tag) {
    m_tag = tag;
}


void Collider::start() {
    auto& engine = Engine::instance();
    auto collDebug = py_get<bool>(engine.getConfig(), "debug_collision", false);
    auto room = engine.getRoom();
    m_engine = room->getRunner<CollisionEngine>();
    //m_engine = Engine::get().GetRunner<ICollisionEngine>();
    if (m_engine == nullptr) {

        //GLIB_FAIL("The room has a collider component but no collision engine is loaded.");
    } else {
        m_engine->add(this);
        m_callbackHandle = m_node->onMove.reg([&] (Node* e) { m_engine->move(this); } );
    }


    // register to move. When the object moves, we notify the collision engine

    if (collDebug) {
        generateDebugMesh();
    }
}

Bounds Collider::getStaticBounds() const {
    auto b = m_staticBounds;
    b.transform(m_node->getWorldMatrix());
    return b;

}

glm::vec4 Collider::bounds() const {
    auto b = getStaticBounds();
    return glm::vec4(b.min.x, b.min.y, b.max.x, b.max.y);
}

std::type_index Collider::getType() {
    return std::type_index(typeid(Collider));
}



SimpleCollider::SimpleCollider(std::shared_ptr<Shape> shape, int flag, int mask, int tag) : Collider(flag, mask, tag), m_shape(shape) {
    m_staticBounds = m_shape->getBounds();
}

void SimpleCollider::generateDebugMesh() {
    if (m_debugNode != nullptr) {
        m_debugNode->remove();
    }
    auto& modelMaker = ModelMaker::instance();
    auto model = modelMaker.make(m_shape, glm::vec4(1.f), FillType::OUTLINE);
    auto node = std::make_shared<Node>();
    node->setModel(model);
    m_node->add(node);
    m_debugNode = node.get();
}