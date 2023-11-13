#include "walk2d.h"
#include "../node.h"
#include "../util.h"


Walk2D::Walk2D(const pybind11::kwargs & kwargs) : Walk(kwargs) {

}



//void Walk2D::start() {
//    auto node = m_sm->getNode();
//    m_node = node;
//    m_controller = dynamic_cast<Controller2D*>(node->getComponent<Controller>());
//    assert(m_controller != nullptr);
//
//    m_dynamics = node->getComponent<Dynamics>();
//    assert(m_dynamics != nullptr);
//
//    //m_animatedRenderer = dynamic_cast<SpriteRenderer*>(m_node->getComponent<Renderer>());
//    m_animatedRenderer = node->getComponent<Renderer>();
//}

void Walk2D::init(const pybind11::kwargs &args) {
	State::init(args);
	auto left = py_get_dict<bool>(args, "left", _keys.x == -1);
	_keys.x = left ? -1 : 1;

}
//
//void Walk2D::setParent(StateMachine * sm) {
//	State::setParent(sm);
//	m_node = sm->getNode();
//
//
//}


void Walk2D::run(double dt) {
	auto dtf = static_cast<float>(dt);

	control();


	float maxSpeed {0.f};
	if (m_controller->grounded()) {
		maxSpeed = m_maxSpeedGround;
		if (_keys.jump) {
			m_dynamics->m_velocity.y = m_jumpVelocity;
			m_isJumping = true;
		} else {
			m_dynamics->m_velocity.y = 0.0f;
			m_isJumping = false;
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

	if (_keys.x != 0) {
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
	if (_keys.x != 0) {
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
		    // jump or fall anim?
			m_animatedRenderer->setAnimation(m_isJumping ? m_jumpAnim : m_fallAnim);
		}


	}
}