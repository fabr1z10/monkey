#include "moveacc.h"
#include "../pyhelper.h"
#include "../node.h"

MoveAccelerated::MoveAccelerated(int id, glm::vec3 velocity, glm::vec3 acceleration, float timeOut) : NodeAction(id) {
	m_initialVelocity = velocity;
	m_velocity = m_initialVelocity;
	m_acceleration = acceleration;
	m_timeOut = timeOut;
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