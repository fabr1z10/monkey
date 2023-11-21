#include "collider.h"
#include "../engine.h"
#include "../error.h"
#include "../runners/collision_engine.h"
#include "../pyhelper.h"
#include "../node.h"
#include "../models/modelmake.h"

using namespace pybind11::literals; // to bring in the `_a` literal

Collider::Collider(const pybind11::kwargs& args) : m_callbackHandle(-1), m_engine(nullptr), m_debugNode(nullptr) {
	m_flag = py_get_dict<int>(args, "flag");
	m_mask = py_get_dict<int>(args, "mask");
	m_tag = py_get_dict<int>(args, "tag");
	_batchId = py_get_dict<std::string>(args, "batch", "");
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
    m_engine = room->getRunner<ICollisionEngine>();
    //m_engine = Engine::get().GetRunner<ICollisionEngine>();
    if (m_engine == nullptr) {

        GLIB_FAIL("The room has a collider component but no collision engine is loaded.");
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



SimpleCollider::SimpleCollider(const pybind11::kwargs& args) : Collider(args) {//std::shared_ptr<Shape> shape, int flag, int mask, int tag) : Collider(flag, mask, tag), m_shape(shape) {
	m_shape = py_get_dict<std::shared_ptr<Shape>>(args, "shape");
    m_staticBounds = m_shape->getBounds();
}

void SimpleCollider::generateDebugMesh() {
    if (m_debugNode != nullptr) {
        m_debugNode->remove();
    }
    auto& modelMaker = ModelMaker::instance();
    auto model = modelMaker.make(m_shape, glm::vec4(1.f), FillType::OUTLINE);
    auto node = std::make_shared<Node>();
    node->setModel(model, pybind11::dict("batch"_a = _batchId));
    m_node->add(node);
    m_debugNode = node.get();
}