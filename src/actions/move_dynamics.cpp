#include "move_dynamics.h"
#include "../pyhelper.h"
#include "../node.h"

MoveDynamics::MoveDynamics(int id, glm::vec3 velocity, glm::vec3 acceleration) : NodeAction(id),
	m_initialVelocity(velocity), m_acceleration(acceleration) {
}


void MoveDynamics::start() {
	NodeAction::start();
	m_controller = dynamic_cast<Controller2D*>(m_node->getComponent<Controller>());

	m_dyn = m_node->getComponent<Dynamics>();
	assert(m_controller != nullptr);
	assert(m_dyn != nullptr);
	m_dyn->m_velocity = m_initialVelocity;

}

int MoveDynamics::process(double dt) {
	auto dtf = static_cast<float>(dt);

	m_dyn->m_velocity += m_acceleration * dtf;
	auto delta = m_dyn->m_velocity * dtf;
	m_controller->move(delta, false);

	// ends when grounded
	if (m_controller->grounded())
		return 0;
	return 1;
}