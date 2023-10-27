#include "walk2d.h"
#include "../node.h"
#include "../util.h"


Walk2D::Walk2D(const pybind11::kwargs & kwargs) : State(), m_controller(nullptr), m_dynamics(nullptr),
    m_node(nullptr), m_animatedRenderer(nullptr), m_left(false), m_right(false), m_up(false), m_down(false), m_jumpVelocity(0.f) {

    m_gravity = py_get_dict<float>(kwargs, "gravity", 0.f);

    // can jump ?
    m_jumpHeight = py_get_dict<float>(kwargs, "jump_height", 0.f);
    m_timeToJumpApex = py_get_dict<float>(kwargs, "time_to_jump_apex", 0.f);


    if (m_jumpHeight > 0.f) {

        // override gravity and jump velocity
        m_gravity = (2.0f * m_jumpHeight) / (m_timeToJumpApex * m_timeToJumpApex);
        m_jumpVelocity = (2.0f * m_jumpHeight) / m_timeToJumpApex;
    }

    //m_jumpVelocity = m_timeToJumpApex == 0.f ? 0.f : (m_jumpHeight + 0.5f * m_gravity * m_timeToJumpApex * m_timeToJumpApex) / m_timeToJumpApex;

    m_maxSpeedGround = py_get_dict<float>(kwargs, "speed", 10.f);
    m_maxSpeedAir = py_get_dict<float>(kwargs, "speed_air", m_maxSpeedGround);
    m_accelerationTime = py_get_dict<float>(kwargs, "acc_time", 0.1f);
    if (m_accelerationTime == 0.f)
        m_acceleration = 0.f;
    else
        m_acceleration = m_maxSpeedGround / m_accelerationTime;

    m_idleAnim = py_get_dict<std::string>(kwargs, "idle", "idle");
    m_walkAnim = py_get_dict<std::string>(kwargs, "walk", "walk");
    m_jumpAnim = py_get_dict<std::string>(kwargs, "jump", "jump");
    m_fallAnim = py_get_dict<std::string>(kwargs, "fall_anim", "jump");
}


void Walk2D::setParent(StateMachine * sm, const pybind11::kwargs & kwargs) {
    m_jmp = false;
    State::setParent(sm, kwargs);

}

void Walk2D::start() {
    auto node = m_sm->getNode();
    m_node = node;
    m_controller = dynamic_cast<Controller2D*>(node->getComponent<Controller>());
    assert(m_controller != nullptr);

    m_dynamics = node->getComponent<Dynamics>();
    assert(m_dynamics != nullptr);

    //m_animatedRenderer = dynamic_cast<SpriteRenderer*>(m_node->getComponent<Renderer>());
    m_animatedRenderer = node->getComponent<Renderer>();
}

void Walk2D::init(const pybind11::kwargs &args) {
	State::init(args);
	m_left = py_get_dict<bool>(args, "left", m_left);
	m_right = !m_left;

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
		if (m_up) {
			m_dynamics->m_velocity.y = m_jumpVelocity;
			m_jmp = true;
		} else {
			m_dynamics->m_velocity.y = 0.0f;
			m_jmp = false;
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
		    // jump or fall anim?
			m_animatedRenderer->setAnimation(m_jmp ? m_jumpAnim : m_fallAnim);
		}


	}
}