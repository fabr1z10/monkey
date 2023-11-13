#include "walk.h"


Keys::Keys() : x(0), z(0), jump(false), crouch(false) {

}

Walk::Walk(const pybind11::kwargs & kwargs) : State(),
	m_controller(nullptr), m_dynamics(nullptr), m_node(nullptr), m_animatedRenderer(nullptr),
	m_jumpVelocity(0.f), m_isJumping(false) {

	m_gravity = py_get_dict<float>(kwargs, "gravity", 0.f);

	// can jump ?
	m_jumpHeight = py_get_dict<float>(kwargs, "jump_height", 0.f);
	m_timeToJumpApex = py_get_dict<float>(kwargs, "time_to_jump_apex", 0.f);

	if (m_jumpHeight > 0.f) {
		// override gravity and jump velocity
		m_gravity = (2.0f * m_jumpHeight) / (m_timeToJumpApex * m_timeToJumpApex);
		m_jumpVelocity = (2.0f * m_jumpHeight) / m_timeToJumpApex;
	}
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

void Walk::setParent(StateMachine * sm, const pybind11::kwargs & kwargs) {
	m_isJumping = false;
	State::setParent(sm, kwargs);
}

void Walk::start() {
	auto node = m_sm->getNode();
	m_node = node;

	m_controller = node->getComponent<Controller>();
	assert(m_controller != nullptr);

	m_dynamics = node->getComponent<Dynamics>();
	assert(m_dynamics != nullptr);

	m_animatedRenderer = node->getComponent<Renderer>();
}