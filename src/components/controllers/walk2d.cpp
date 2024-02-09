#include "walk2d.h"
#include "../../util.h"
#include "../../node.h"
#include <GLFW/glfw3.h>
#include "../../pyhelper.h"


extern GLFWwindow * window;


Walk2D::Walk2D(float maxSpeedGround, float accelerationTime, float jumpHeight, float timeToJumpApex,
			   const pybind11::kwargs& args) : Component(), _jumping(false), _jumpHeight(jumpHeight), _timeToJumpApex(timeToJumpApex),
			   _maxSpeedGround(maxSpeedGround), _accelerationTime(accelerationTime), _a(glm::vec3(0.f)), _v(glm::vec3(0.f))
{
	_gravity = py_get_dict<float>(args, "gravity", 0.f);
	if (_jumpHeight > 0.f) {
		// override gravity and jump velocity
		_gravity = (2.0f * _jumpHeight) / (_timeToJumpApex * _timeToJumpApex);
		_jumpVelocity = (2.0f * _jumpHeight) / _timeToJumpApex;
	}

	if (_accelerationTime == 0.f)
		_acceleration = 0.f;
	else
		_acceleration = _maxSpeedGround / _accelerationTime;
	_maxSpeedAir = py_get_dict<float>(args, "max_speed_air", _maxSpeedGround);
}

PlayerWalk2D::PlayerWalk2D(float maxSpeedGround, float accelerationTime, float jumpHeight, float timeToJumpApex,
	const pybind11::kwargs &args) : Walk2D(maxSpeedGround, accelerationTime, jumpHeight,
	timeToJumpApex, args) {
}

void Walk2D::start() {
	_controller = dynamic_cast<Controller2D*>(m_node->getComponent<Controller>());
	assert(_controller != nullptr);
}

void Walk2D::update(double dt) {

	auto dtf = static_cast<float>(dt);

	control();

	float maxSpeed {0.f};
	if (_controller->grounded()) {
		maxSpeed = _maxSpeedGround;
		if (_flags & 4) {
			_v.y = _jumpVelocity;
			_jumping = true;
		} else {
			_v.y = 0.0f;
			_jumping = false;
		}
	} else {
		// bump head
		maxSpeed = _maxSpeedAir;
		if (_controller->ceiling()) {
			_v.y = 0;
		}
	}

	_a = glm::vec3(0.f, -_gravity, 0.f);

	if (_flags & 3) {
		_a.x = _acceleration;
	} else {
		// apply deceleration only if velocity above threshold
		if (fabs(_v.x) > 0.1f) {
			_a.x = -signf(_v.x) * _acceleration;
		} else {
			_a.x = 0.0f;
			_v.x = 0.0f;
		}
	}

	_v += _a * dtf;
	auto delta = _v * dtf;

	// limit horizontal vel to max speed
	if (_flags & 3) {
		if (fabs(_v.x) > maxSpeed) {
			_v.x = signf(_v.x) * maxSpeed;
		}
	}

	_controller->move(delta, false);

	// update animation, if we have a sprite renderer
//	if (m_animatedRenderer) {
//		if (m_controller->grounded()) {
//			if (fabs(m_dynamics->m_velocity.x) < 0.1f) {
//				m_animatedRenderer->setAnimation(m_idleAnim);
//			} else {
//				m_animatedRenderer->setAnimation(m_walkAnim);
//			}
//		} else {
//			// jump or fall anim?
//			m_animatedRenderer->setAnimation(m_isJumping ? m_jumpAnim : m_fallAnim);
//		}
//	}

}

void PlayerWalk2D::control() {

	_flags = 0;
	_flags |= glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS ? 1 : 0;
	_flags |= glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS ? 2 : 0;
	_flags |= glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS ? 4 : 0;
	_flags |= glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS ? 8 : 0;
	_flags |= glfwGetKey(window, _jmpKey) == GLFW_PRESS ? 16 : 0;
	//std::cout << _flags << "\n";
	//m_node->setFlipX(_flags & 2);
}

int PlayerWalk2D::keyCallback(GLFWwindow *, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_LEFT) {
			m_node->setFlipX(true);
		} else if (key == GLFW_KEY_RIGHT) {
			m_node->setFlipX(false);
		}
	}
	//std::cout << m_node->getFlipX() << "\n";
}