#include "walk2d.h"
#include "../node.h"
#include "../util.h"


Walk2D::Walk2D(const std::string& id, const pybind11::kwargs& kwargs) : State(id, kwargs) {
	m_gravity = py_get_dict<float>(kwargs, "gravity", 0.0f);
	m_jumpHeight = py_get_dict<float>(kwargs, "jump_height");
	m_timeToJumpApex = py_get_dict<float>(kwargs, "time_to_jump_apex");
	m_jumpVelocity = (m_jumpHeight + 0.5f * m_gravity * m_timeToJumpApex * m_timeToJumpApex) / m_timeToJumpApex;
	m_maxSpeedGround = py_get_dict<float>(kwargs, "speed");
	m_maxSpeedAir = py_get_dict<float>(kwargs, "speed_air", m_maxSpeedGround);
	m_accelerationTime = py_get_dict<float>(kwargs, "acc_time", 0.1f);
	if (m_accelerationTime == 0.f)
		m_acceleration = 0.f;
	else
		m_acceleration = m_maxSpeedGround / m_accelerationTime;
	m_idleAnim = py_get_dict<std::string>(kwargs, "idle_anim", "idle");
	m_walkAnim = py_get_dict<std::string>(kwargs, "walk_anim", "walk");
	m_jumpAnim = py_get_dict<std::string>(kwargs, "jump_anim", "jump");
}

void Walk2D::init(const pybind11::kwargs &args) {
	State::init(args);
	m_left = py_get_dict<bool>(args, "left", m_left);
	m_right = !m_left;

}

void Walk2D::setParent(StateMachine * sm) {
	State::setParent(sm);
	m_node = sm->getNode();

	m_controller = dynamic_cast<Controller2D*>(m_sm->getNode()->getComponent<Controller>());
	assert(m_controller != nullptr);

	m_dynamics = m_sm->getNode()->getComponent<Dynamics>();
	assert(m_dynamics != nullptr);

	m_animatedRenderer = dynamic_cast<SpriteRenderer*>(m_node->getComponent<Renderer>());
}


void Walk2D::run(double dt) {
	auto dtf = static_cast<float>(dt);

	control();


	float maxSpeed {0.f};
	if (m_controller->grounded()) {
		maxSpeed = m_maxSpeedGround;
		if (m_up) {
			m_dynamics->m_velocity.y = m_jumpVelocity;
		} else {
			m_dynamics->m_velocity.y = 0.0f;
		}
	} else {
		// bump head
		maxSpeed = m_maxSpeedAir;
		if (m_controller->ceiling()) {
			m_dynamics->m_velocity.y = 0;
		}

	}


	glm::vec3 a(0.0f);
	a.y = -m_gravity;

	if (m_left || m_right) {
		a.x = m_acceleration; // (m_left ? -1.f : 1.f) * m_acceleration;
	} else {
		// apply deceleration only if velocity above threshold
		if (fabs(m_dynamics->m_velocity.x) > 0.1f) {
			a.x = - signf(m_dynamics->m_velocity.x) * m_acceleration;
		} else {
			a.x = 0.0f;
			m_dynamics->m_velocity.x = 0.0f;
		}
	}

	m_dynamics->m_velocity += a * dtf;

	// limit horizontal vel to max speed
	if (m_left || m_right) {
		if (fabs(m_dynamics->m_velocity.x) > maxSpeed) {
			m_dynamics->m_velocity.x = signf(m_dynamics->m_velocity.x) * maxSpeed;
		}
	}


	auto delta = m_dynamics->m_velocity * dtf;

	m_controller->move(delta, false);

	// update animation, if we have a sprite renderer
	if (m_animatedRenderer) {
		if (m_controller->grounded()) {
			if (fabs(m_dynamics->m_velocity.x) < 0.1f) {
				m_animatedRenderer->setAnimation(m_idleAnim);
			} else {
				m_animatedRenderer->setAnimation(m_walkAnim);
			}
		} else {
			m_animatedRenderer->setAnimation(m_jumpAnim);
		}


	}
}