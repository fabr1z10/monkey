#include "controller.h"
#include "../node.h"
#include <glm/gtx/matrix_decompose.hpp>
#include "../util.h"
#include "../pyhelper.h"
#include "../engine.h"
#include "platform.h"
#include <glm/gtx/transform.hpp>
//#include "../shapes/shapemodel.h"
using namespace pybind11::literals; // to bring in the `_a` literal


Controller::Controller(const pybind11::kwargs& args) : m_debugShape(nullptr) {
	m_size = py_get_dict<glm::vec3>(args, "size", glm::vec3(0.f));
	m_center = py_get_dict<glm::vec3>(args, "center", glm::vec3(m_size.x * 0.5f, 0.f, 0.f));
    _batchId = py_get_dict<std::string>(args, "batch", "");
	m_maskDown = py_get_dict<int>(args, "mask_down", 2);
	m_maskUp = py_get_dict<int>(args, "mask_up", m_maskDown);
    computeCoordinates();
}


void Controller::start() {
	auto& engine = Engine::instance();
	auto room = engine.getRoom();
	m_collisionEngine = room->getRunner<CollisionEngine>();
	setDebugShape();
}

void Controller::setDebugShape() {
	auto& engine = Engine::instance();
	auto collDebug = py_get<bool>(engine.getConfig(), "debug_collision", false);
	if (collDebug) {
		if (m_debugShape != nullptr) {
			m_debugShape->remove();
		}
		auto node = std::make_shared<Node>();
		auto model = getDebugModel();
		//node->setModel(model);
        node->setModel(model, pybind11::dict("batch"_a = _batchId));
        m_node->add(node);
		m_debugShape = node.get();
	}
}


void Controller::setSize(glm::vec3 size, glm::vec3 center) {
	m_size = size;
	m_center = center;
	setDebugShape();
	computeCoordinates();
}


void Controller::computeCoordinates() {
	// raycast origins in local coordinates. This won't change until shape is changed
	m_localTopFwd = m_size - m_center;
	m_localTopBack = glm::vec3(0.0f, m_size.y, 0.0f) - m_center;
	m_localBottomFwd = glm::vec3(m_size.x, 0.0f, 0.0f) - m_center;
	m_localBottomBack = glm::vec3(0.0f) - m_center;
}


void Controller::setMask(int up, int down) {
	m_maskUp = up;
	m_maskDown = down;
}

glm::ivec2 Controller::getMask() const {
	return glm::ivec2(m_maskUp, m_maskDown);
}




void Controller::move(glm::vec3 & delta, bool) {
	m_node->move(glm::translate(delta));
}

glm::vec3 Controller::getSize() const {
	return m_size;
}

void Controller::resetCollisions() {
	m_collisionStatus = 0u;
}

