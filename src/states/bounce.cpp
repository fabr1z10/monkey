#include "bounce.h"
#include "../pyhelper.h"
#include "../node.h"
#include "../util.h"

Bounce::Bounce(const std::string& id, const pybind11::kwargs& kwargs) : State(id, kwargs) {
	m_gravity = py_get_dict<float>(kwargs, "gravity", 0.0f);
	m_isFixedVel = false;
	if (kwargs.contains("bounce_velocity")) {
		m_fixedVelocityBounce = kwargs["bounce_velocity"].cast<float>();
		m_isFixedVel = true;
	}
	if (kwargs.contains("on_bounce_y")) {
		m_onBounceY = kwargs["on_bounce_y"].cast<pybind11::function>();
	}
	m_checkWalls = py_get_dict<bool>(kwargs, "check_walls", true);
	m_animation = py_get_dict<std::string>(kwargs, "animation", "");
}

void Bounce::setParent(StateMachine * sm) {
	State::setParent(sm);
	m_node = sm->getNode();

	m_controller = dynamic_cast<Controller2D*>(m_sm->getNode()->getComponent<Controller>());
	assert(m_controller != nullptr);

	m_dynamics = m_sm->getNode()->getComponent<Dynamics>();
	assert(m_dynamics != nullptr);

	m_animatedRenderer = dynamic_cast<SpriteRenderer*>(m_node->getComponent<Renderer>());
}


void Bounce::init(const pybind11::kwargs& args) {
	State::init(args);
	auto velocity = py_get_dict<glm::vec2>(args, "velocity");
	//auto anim = dictget<std::string>(args, "anim", "");
	if (!m_animation.empty()) {
		m_animatedRenderer->setAnimation(m_animation);
	}
	m_dynamics->m_velocity = glm::vec3(velocity, 0.f);
	m_bounceCountY = 0;
	m_controller->resetDetails();
	m_node->setFlipX(false);
}


void Bounce::run(double dt) {
	auto dtf = static_cast<float>(dt);


	float maxSpeed {0.f};
	if (m_controller->grounded() || m_controller->ceiling()) {
		m_bounceCountY ++;
		if (m_onBounceY) {
			m_onBounceY(m_node, m_bounceCountY);
		}

		m_dynamics->m_velocity.y = m_isFixedVel ? -sign(m_dynamics->m_velocity.y) * m_fixedVelocityBounce : -m_dynamics->m_velocity.y;
	}
	if (m_controller->left() || m_controller->right()) {
		m_dynamics->m_velocity.x = -m_dynamics->m_velocity.x;
	}

	glm::vec3 a(0.0f);
	a.y = -m_gravity;
	m_dynamics->m_velocity += a * dtf;
	auto delta = m_dynamics->m_velocity * dtf;

	if (m_checkWalls) {
		m_controller->move(delta, false);
	} else {
		m_node->move(delta);
	}

}