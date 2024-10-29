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


Controller::Controller(const pybind11::kwargs& args) : Component(args), m_debugShape(nullptr), m_collisionStatus(0) {
	m_size = py_get_dict<glm::vec2>(args, "size", glm::vec2(0.f));
	m_center = py_get_dict<glm::vec2>(args, "center", glm::vec2(m_size.x * 0.5f, 0.f));
    //_batchId = py_get_dict<std::string>(args, "batch", "");
	m_maskDown = py_get_dict<int>(args, "mask_down", 2 | 32);
	m_maskUp = py_get_dict<int>(args, "mask_up", 2);
    computeCoordinates();
}


void Controller::start() {
	auto& engine = Engine::instance();
	auto room = engine.getRoom();
	m_collisionEngine = room->getRunner<ICollisionEngine>();
	setDebugShape();
}

void Controller::setDebugShape() {
	auto& engine = Engine::instance();
	//auto collDebug = py_get<bool>(engine.getConfig(), "debug_collision", false);
	if (engine.drawColliderOutline()) {
        auto batchId = Engine::instance().getColliderOutlineBatch();

        if (m_debugShape != nullptr) {
			m_debugShape->remove();
		}
		auto node = std::make_shared<Node>();
		auto model = getDebugModel();
		//node->setModel(model);
        node->setModel(model, pybind11::dict("batch"_a = batchId));
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
	m_localTopBack = glm::vec2(0.0f, m_size.y) - m_center;
	m_localBottomFwd = glm::vec2(m_size.x, 0.0f) - m_center;
	m_localBottomBack = glm::vec2(0.0f) - m_center;
}


void Controller::setMask(int up, int down) {
	m_maskUp = up;
	m_maskDown = down;
}

glm::ivec2 Controller::getMask() const {
	return glm::ivec2(m_maskUp, m_maskDown);
}




void Controller::move(glm::vec2 & delta, bool) {
	m_node->move(glm::translate(glm::vec3(delta, 0.f)));
}

glm::vec2 Controller::getSize() const {
	return m_size;
}

void Controller::resetCollisions() {
	m_collisionStatus = 0u;
}

