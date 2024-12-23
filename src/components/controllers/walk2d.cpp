//#include "walk2d.h"
//#include "../../util.h"
//#include "../../node.h"
//#include <GLFW/glfw3.h>
//#include "../../pyhelper.h"
//
//
//extern GLFWwindow * window;
//
//
//Walk2D::Walk2D(float maxSpeedGround, float accelerationTime, float jumpHeight, float timeToJumpApex,
//			   const pybind11::kwargs& args) : Component(args), _jumpHeight(jumpHeight), _timeToJumpApex(timeToJumpApex),
//			   _maxSpeedGround(maxSpeedGround), _accelerationTime(accelerationTime), _a(glm::vec3(0.f)), _v(glm::vec3(0.f)), _direction(0), _dir(1.f)
//{
//	_gravity = py_get_dict<float>(args, "gravity", 0.f);
//	if (_jumpHeight > 0.f) {
//		// override gravity and jump velocity
//		_gravity = (2.0f * _jumpHeight) / (_timeToJumpApex * _timeToJumpApex);
//		_jumpVelocity = (2.0f * _jumpHeight) / _timeToJumpApex;
//	}
//
//	if (_accelerationTime == 0.f)
//		_acceleration = 0.f;
//	else
//		_acceleration = _maxSpeedGround / _accelerationTime;
//	//_maxSpeedAir = py_get_dict<float>(args, "max_speed_air", _maxSpeedGround);
//
//	// flip horizontally when moving left
//	_flipH = py_get_dict<bool>(args, "flip_h", true);
//}
//
//// you can allow for double jump here
//void Walk2D::jump() {
//    if (_controller->grounded()) {
//        _v.y = _jumpVelocity;
//    }
//
//}
//
//
//PlayerWalk2D::PlayerWalk2D(float maxSpeedGround, float accelerationTime, float jumpHeight, float timeToJumpApex,
//	const pybind11::kwargs &args) : Walk2D(maxSpeedGround, accelerationTime, jumpHeight,
//	timeToJumpApex, args) {
//    _jmpKey = py_get_dict<int>(args, "jmp_key", GLFW_KEY_UP);
//}
//
//CustomWalk2D::CustomWalk2D(float maxSpeedGround, float accelerationTime, float jumpHeight, float timeToJumpApex,
//    const pybind11::object internal, const pybind11::kwargs &args) : Walk2D(maxSpeedGround, accelerationTime, jumpHeight, timeToJumpApex, args), _internal(internal) {
//    _internalControl = _internal.attr("update").cast<pybind11::function>();
//
//}
//
//void CustomWalk2D::control() {
//    _internalControl(this);
//}
//
//int CustomWalk2D::keyCallback(GLFWwindow *, int key, int scancode, int action, int mods) {
//    if (action == GLFW_PRESS) {
//        _internal.attr("onKeyPress")(key);
//    }
//
//}
//
//FoeWalk2D::FoeWalk2D(float maxSpeedGround, float accelerationTime, float jumpHeight, float timeToJumpApex,
//    const pybind11::kwargs &args) : Walk2D(maxSpeedGround, accelerationTime, jumpHeight, timeToJumpApex, args) {
//    _flipPlatformEdge = py_get_dict<bool>(args, "flip_platform_edge", true);
//    //_flipH = py_get_dict<bool>(args, "flip_h", true);
//    _direction = py_get_dict<int>(args, "dir", -1);
//}
//
//
//void Walk2D::start() {
//	_controller = dynamic_cast<Controller2D*>(m_node->getComponent<Controller>());
//	assert(_controller != nullptr);
//}
//
//void CustomWalk2D::start() {
//    Walk2D::start();
//    _internal.attr("start")(this);
//}
//void Walk2D::update(double dt) {
//
//	auto dtf = static_cast<float>(dt);
//
//	control();
//
//	if (_flipH && _direction != 0) {
//        m_node->setFlipX(_direction == -1);
//    }
//
//	float maxSpeed {0.f};
//	if (_controller->grounded()) {
//		maxSpeed = _maxSpeedGround;
//		_v.y = std::max(0.f, _v.y);
//	} else {
//		// bump head
//		maxSpeed = _maxSpeedGround;
//		if (_controller->ceiling()) {
//			_v.y = 0;
//		}
//	}
//
//	_a = glm::vec3(0.f, -_gravity, 0.f);
//
//	if (_direction != 0) {
//	    // x-acceleration should be always positive unless when !fliph && dir==-1
//		_a.x = (!_flipH && _direction == -1) ? -_acceleration : _acceleration;
//	} else {
//		// apply deceleration only if velocity above threshold
//		if (fabs(_v.x) > (_acceleration * dtf) + 0.1f) {
//		    // decel, acceleration should be opposite velocity
//			_a.x = -signf(_v.x) * _acceleration;
//		} else {
//			_a.x = 0.0f;
//			_v.x = 0.0f;
//		}
//	}
//
//	_v += _a * dtf;
//	// limit horizontal vel to max speed
//    if (fabs(_v.x) > maxSpeed) {
//		_v.x = signf(_v.x) * maxSpeed;
//	}
//
//    auto delta = _v * dtf;
//	_controller->move(delta, false);
//
//	// update animation, if we have a sprite renderer
////	if (m_animatedRenderer) {
////		if (m_controller->grounded()) {
////			if (fabs(m_dynamics->m_velocity.x) < 0.1f) {
////				m_animatedRenderer->setAnimation(m_idleAnim);
////			} else {
////				m_animatedRenderer->setAnimation(m_walkAnim);
////			}
////		} else {
////			// jump or fall anim?
////			m_animatedRenderer->setAnimation(m_isJumping ? m_jumpAnim : m_fallAnim);
////		}
////	}
//
//}
//
//
//void PlayerWalk2D::control() {
//    bool left = glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS;
//    bool right = glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS;
//    if (!left && !right) {
//        _direction = 0;
//    } else if (left && !right) {
//        _direction = -1;
//    } else {
//        _direction = 1;
//    }
//
//
//    if (glfwGetKey(window, _jmpKey) == GLFW_PRESS && _controller->grounded()) {
//        _v.y = _jumpVelocity;
//    }
//
//}
//
//bool CustomWalk2D::isPressed(int key) {
//    return glfwGetKey(window, key) == GLFW_PRESS;
//}
//
//void FoeWalk2D::control() {
//
////    _flags |= 6;     // foe always moves
////    bool faceLeft = _flags & 1;
////    if (_flipH) {
////        m_node->setFlipX(faceLeft);
////    } else {
////        _dir = (faceLeft) ? -1.f : 1.f;
////    }
//
//    if (_direction == -1 && _controller->left()) {
//        _direction = 1;
//    } else if (_direction == 1 && _controller->right()) {
//        _direction = -1;
//    }
//
////    if (_controller->grounded()) {
////        if (_flipPlatformEdge && _controller->isFalling(_dir)) {
////            _flags ^= 0x01;
////        } else if ((faceLeft && _controller->left()) || (!faceLeft && _controller->right())) {
////            _flags ^= 0x01;
////        }
////
////    }
//
//}
//
//int PlayerWalk2D::keyCallback(GLFWwindow *, int key, int scancode, int action, int mods) {
////	if (action == GLFW_PRESS) {
////		if (key == GLFW_KEY_LEFT) {
////			//m_node->setFlipX(true);
////			_flags |= 1; // set LSB
////		} else if (key == GLFW_KEY_RIGHT) {
////			_flags &= 0xFE; // reset LSB
////		}
////
////
////	}
//
//	//std::cout << m_node->getFlipX() << "\n";
//	return 0;
//}
