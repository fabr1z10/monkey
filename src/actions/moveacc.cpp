#include "moveacc.h"
#include "../pyhelper.h"
#include "../node.h"

MoveAccelerated::MoveAccelerated(const pybind11::kwargs& args) : NodeAction(args) {
	m_initialVelocity = py_get_dict<glm::vec3>(args, "velocity");
	m_velocity = m_initialVelocity;
	m_acceleration = py_get_dict<glm::vec3>(args, "acceleration");
	m_timeOut = py_get_dict<float>(args, "timeout", std::numeric_limits<float>::infinity());
	m_time = 0.f;
}


int MoveAccelerated::process(double dt) {
	auto dtf = static_cast<float>(dt);
	if (m_time > m_timeOut) {
		return 0;
	}
	glm::vec2 delta = m_velocity * dtf;
	m_velocity += m_acceleration * dtf;
	m_node->move(glm::vec3(delta, 0.f));
	m_time += dtf;
	return 1.f;
}