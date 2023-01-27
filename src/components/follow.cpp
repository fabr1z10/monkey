#include "follow.h"
#include "../node.h"

Follow::Follow (std::shared_ptr<Camera> cam, pybind11::tuple& c, pybind11::tuple& a) : Component(), m_cam(cam) {
	m_relativePos = glm::vec3(c[0].cast<float>(), c[1].cast<float>(), c[2].cast<float>());
	m_up = glm::vec3(a[0].cast<float>(), a[1].cast<float>(), a[2].cast<float>());+
																						 m_previous = glm::vec3(0.0f);
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