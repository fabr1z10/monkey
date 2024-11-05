#include "collider.h"
#include "../engine.h"
#include "../error.h"
#include "../runners/collision_engine.h"
#include "../pyhelper.h"
#include "../node.h"
#include "../models/modelmake.h"

using namespace pybind11::literals; // to bring in the `_a` literal
using namespace shapes;


Collider::Collider(int flag, int mask, int tag, const pybind11::kwargs& args) : Component(args),
    m_callbackHandle(-1), m_engine(nullptr), m_debugNode(nullptr), m_flag(flag), m_mask(mask), m_tag(tag) {

}

void Collider::shutdown() {
	_response.clear();
}

bool Collider::respondTo(Collider * other) {
	return (_response.count(other->getCollisionTag()) > 0);
}

void Collider::setResponse(int tag, const pybind11::kwargs& args) {
	_response[tag] = {
			py_get_dict<pybind11::function>(args, "on_enter", pybind11::function()),
			py_get_dict<pybind11::function>(args, "on_exit", pybind11::function()),
			py_get_dict<pybind11::function>(args, "on_continue", pybind11::function())};


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
    //auto collDebug = py_get<bool>(engine.getConfig(), "debug_collision", false);
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

    if (engine.drawColliderOutline()) {
        generateDebugMesh();
    }
}

void Collider::startCollision(Collider * c, glm::vec2 delta) {
	//if (_current.count(c) == 0) {
	//	_current.insert(c);
		auto it = _response.find(c->getCollisionTag());
		if (it != _response.end() && it->second.onStart) {
			it->second.onStart(getNode(), c->getNode(), delta);
		}
	//}
}

void Collider::endCollision(Collider * c) {
	//if (_current.count(c) > 0) {
	//	_current.erase(c);
		auto it = _response.find(c->getCollisionTag());
		if (it != _response.end() && it->second.onEnd) {
			it->second.onEnd(getNode(), c->getNode());
		}

	//}
}

void Collider::update(double) {
//	for (auto* c : _previous) {
//		auto it = _response.find(c->getCollisionTag());
//		if (_current.count(c) == 0) {
//			if (it != _response.end() && it->second.onEnd) {
//				it->second.onEnd(getNode(), c->getNode());
//			}
//		}
//	}
//
//	for (auto* c : _current) {
//		auto it = _response.find(c->getCollisionTag());
//		if (it == _response.end()) {
//			continue;
//		}
//		if (_previous.count(c) > 0) {
//			if (it->second.onContinue) {
//				it->second.onContinue(getNode(), c->getNode());
//			}
//		} else {
//			if (it->second.onStart) {
//				it->second.onStart(getNode(), c->getNode());
//			}
//		}
//	}
//
//	_previous = _current;
//	_current.clear();

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



SimpleCollider::SimpleCollider(int flag, int mask, int tag, std::shared_ptr<Shape> shape, const pybind11::kwargs& args) :
    Collider(flag, mask, tag, args), m_shape(shape) {//std::shared_ptr<Shape> shape, int flag, int mask, int tag) : Collider(flag, mask, tag), m_shape(shape) {
	//m_shape = py_get_dict<std::shared_ptr<Shape>>(args, "shape");
    m_staticBounds = m_shape->getBounds();
}

void SimpleCollider::generateDebugMesh() {
    if (m_debugNode != nullptr) {
        m_debugNode->remove();
    }
    auto batchId = Engine::instance().getColliderOutlineBatch();
    auto& modelMaker = ModelMaker::instance();
    auto model = modelMaker.make(batchId, m_shape, glm::vec4(1.f), FillType::OUTLINE);
    auto node = std::make_shared<Node>();
    node->setModel(model);//pybind11::dict("batch"_a = _batchId));
    m_node->add(node);
    m_debugNode = node.get();
}