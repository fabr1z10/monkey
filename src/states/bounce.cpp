#include "bounce.h"
#include "../pyhelper.h"
#include "../node.h"
#include "../util.h"

Bounce::Bounce(const std::string& id, const pybind11::kwargs& kwargs) : State(id, kwargs) {
	m_gravity = py_get_dict<float>(kwargs, "gravity", 0.0f);
	m_isFixedVel = false;
	if (kwargs.contains("bounce_velocity")) {
		m_fixedVelocityBounce = kwargs["bounce_velocity"].cast<std::vector<float>>();
		m_isFixedVel = true;
	}
	if (kwargs.contains("on_bounce_y") && !kwargs["on_bounce_y"].is_none()) {
		m_onBounceY = kwargs["on_bounce_y"].cast<pybind11::function>();
	}
	m_checkWalls = py_get_dict<bool>(kwargs, "check_walls", true);
	m_animation = py_get_dict<std::string>(kwargs, "animation", "");
	m_flipHorizontally = py_get_dict<bool>(kwargs, "flip", false);
	m_horizontalSpeed = py_get_dict<float>(kwargs, "speed");
	m_left = py_get_dict<bool>(kwargs, "left", true);

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
	auto velocity = py_get_dict<glm::vec2>(args, "velocity", glm::vec2(m_horizontalSpeed, 0.f));
	//auto anim = dictget<std::string>(args, "anim", "");
	if (!m_animation.empty()) {
		m_animatedRenderer->setAnimation(m_animation);
	}
	m_dynamics->m_velocity = glm::vec3(velocity, 0.f);
	m_bounceCountY = 0;
	m_controller->resetDetails();
	m_counter = 0;

	m_left = py_get_dict<bool>(args, "left", m_left);
	m_node->setFlipX(m_left);
}


void Bounce::run(double dt) {
	auto dtf = static_cast<float>(dt);


	float maxSpeed {0.f};
	if (m_controller->grounded() || m_controller->ceiling()) {
		m_bounceCountY ++;
		if (m_onBounceY) {
			m_onBounceY(m_node, m_bounceCountY);
		}
		if (m_isFixedVel) {
			m_dynamics->m_velocity.y = -signf(m_dynamics->m_velocity.y) * m_fixedVelocityBounce[m_counter];
			m_counter ++;
			if (m_counter > m_fixedVelocityBounce.size()) {
				m_counter = 0;
			}
		} else {
			m_dynamics->m_velocity.y = -m_dynamics->m_velocity.y;
		}

	}
	if (m_controller->left() || m_controller->right()) {
		m_left = !m_left;
		//m_dynamics->m_velocity.x = -m_dynamics->m_velocity.x;
	}
	if (m_flipHorizontally) {
		m_node->setFlipX(m_left);

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