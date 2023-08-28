#include "follow.h"
#include "../node.h"
#include "../engine.h"
#include "../pyhelper.h"

Follow::Follow (const pybind11::kwargs& args) : Component() {
	_camId = py_get_dict<int>(args, "cam");
	m_relativePos = py_get_dict<glm::vec3>(args, "pos");
	m_up = py_get_dict<glm::vec3>(args, "up", glm::vec3(0,1,0));
	m_previous = glm::vec3(0.0f);
}

void Follow::start() {
	m_cam = Engine::instance().getRoom()->getCamera(_camId);
	auto worldPos = m_node->getWorldPosition();
	glm::vec3 eye = worldPos + m_relativePos;
	auto dir = glm::vec3(0.f, 0.f, -1.f);//(glm::normalize(m_node->getWorldPosition() - eye);
	m_cam->setPosition(eye, dir, m_up);
	m_previous = worldPos;
	
}


void Follow::update (double dt) {
	if (!m_active) return;

	auto worldPos = m_node->getWorldPosition();
	if (m_previous - worldPos != glm::vec3(0.0f)) {
		glm::vec3 eye = worldPos + m_relativePos;
		auto dir = glm::vec3(0.f, 0.f, -1.f);//(glm::normalize(m_node->getWorldPosition() - eye);
		m_cam->setPosition(eye, dir, m_up);
		m_previous = worldPos;
	}

}