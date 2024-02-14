#include "walk2d.h"
#include "../../util.h"
#include "../../node.h"
#include <GLFW/glfw3.h>
#include "../../pyhelper.h"


extern GLFWwindow * window;


Walk2D::Walk2D(float maxSpeedGround, float accelerationTime, float jumpHeight, float timeToJumpApex,
			   const pybind11::kwargs& args) : Component(), _jumping(false), _jumpHeight(jumpHeight), _timeToJumpApex(timeToJumpApex),
			   _maxSpeedGround(maxSpeedGround), _accelerationTime(accelerationTime), _a(glm::vec3(0.f)), _v(glm::vec3(0.f)), _flags(0), _dir(1.f)
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
    _jmpKey = py_get_dict<int>(args, "jmp_key", GLFW_KEY_UP);
}


FoeWalk2D::FoeWalk2D(float maxSpeedGround, float accelerationTime, float jumpHeight, float timeToJumpApex,
    const pybind11::kwargs &args) : Walk2D(maxSpeedGround, accelerationTime, jumpHeight, timeToJumpApex, args) {
    _flipPlatformEdge = py_get_dict<bool>(args, "flip_platform_edge", true);
    _flipH = py_get_dict<bool>(args, "flip_h", true);
    auto initialDirection = py_get_dict<int>(args, "dir", -1);
    if (initialDirection < 0) {
        _flags |= 1;
    }

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
		_v.y = std::max(0.f, _v.y);
	} else {
		// bump head
		maxSpeed = _maxSpeedAir;
		if (_controller->ceiling()) {
			_v.y = 0;
		}
	}

	_a = glm::vec3(0.f, -_gravity, 0.f);

	if (_flags & 2) {
	    // is moving
		_a.x = (_flags & 4 ? 1.f : -1.f) * _acceleration * _dir;
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
	if (_flags & 2) {
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

	//_flags = 0;
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
	    _flags |= 6;
	} else {
	    _flags &= 0xFD;
	}

    if (glfwGetKey(window, _jmpKey) == GLFW_PRESS && _controller->grounded()) {
        _v.y = _jumpVelocity;
    }

}

void FoeWalk2D::control() {
    _flags |= 6;     // foe always moves
    bool faceLeft = _flags & 1;
    if (_flipH) {
        m_node->setFlipX(faceLeft);
    } else {
        _dir = (faceLeft) ? -1.f : 1.f;
    }
    if (_controller->grounded()) {
        if (_flipPlatformEdge && _controller->isFalling(_dir)) {
            _flags ^= 0x01;
        } else if ((faceLeft && _controller->left()) || (!faceLeft && _controller->right())) {
            _flags ^= 0x01;
        }

    }

}

int PlayerWalk2D::keyCallback(GLFWwindow *, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_LEFT) {
			//m_node->setFlipX(true);
			_flags |= 1; // set LSB
		} else if (key == GLFW_KEY_RIGHT) {
			_flags &= 0xFE; // reset LSB
		}


	}
    m_node->setFlipX((_flags& 1) > 0);
	//std::cout << m_node->getFlipX() << "\n";
}
