#include "sierra2d.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include "../../node.h"
#include "../../error.h"
#include "../../engine.h"
#include "../../pyhelper.h"


extern GLFWwindow* window;

using namespace shapes;


Sierra2DController::Sierra2DController(const pybind11::kwargs &args) : Component(args) {
	_direction = py_get_dict<std::string>(args, "dir", "e");

    //_yFront = py_get_dict<float>(args, "y_front");
    //_yBack = py_get_dict<float>(args, "y_back");
    //_zFunc = py_get_dict<pybind11::function>(args, "z_func", pybind11::function());
    //_scaleFunc = py_get_dict<pybind11::function>(args, "scale_func", pybind11::function());

    //_a = 2.f / (_yFront - _yBack);
    //_b = (_yBack + _yFront) / (_yBack - _yFront);
}

//std::type_index Sierra2DController::getType() {
//	return std::type_index(typeid(Sierra2DController));
//}


PlayerSierra2DController::PlayerSierra2DController(const pybind11::kwargs &args) : Sierra2DController(args), _enableControls(true) {
	_halfWidth = py_get_dict<float>(args, "half_width");
	_skinWidth = py_get_dict<float>(args, "skinWidth", .015f);
	_idleAnimation = py_get_dict<std::string>(args, "idle", "idle");
	_walkAnimation = py_get_dict<std::string>(args, "walk", "walk");
	_speed = py_get_dict<float>(args, "speed");
}

void PlayerSierra2DController::setAnim(const std::string &idle, const std::string &walk) {
	_idleAnimation = idle;
	_walkAnimation = walk;
}


void PlayerSierra2DController::enable(bool value) {
	_enableControls = value;
}

void Sierra2DController::start() {
	auto& engine = Engine::instance();
	auto room = engine.getRoom();
	_walkManager = room->getRunner<WalkManager>();

}

void PlayerSierra2DController::start() {
	//_dir = 'e';
	_lookingLeft = false;
	m_animatedRenderer = m_node->getComponent<Renderer>();

	Sierra2DController::start();
	auto& engine = Engine::instance();
	auto room = engine.getRoom();
	m_collisionEngine = room->getRunner<ICollisionEngine>();
	M_Assert(m_collisionEngine != nullptr, "The component PlayerSierra2DController requires a collision engine.");
	auto dir = _direction == "w" ? "e" : _direction;
	m_animatedRenderer->setAnimation(_idleAnimation + "_" +dir);
	m_node->setFlipX(_direction == "w");
	std::cout << "position = " << m_node->getX() << ", " << m_node->getY() << ", " << _direction << "\n";
}

void Sierra2DController::update(double) {

    auto currentPos = m_node->getWorldPosition();
    //if (currentPos != _previousPosition) {

        updateZ(currentPos.x, currentPos.y);

        _previousPosition = currentPos;
    //}

}



void Sierra2DController::updateZ(float x, float y) {
	if (_walkManager != nullptr) {
		auto z = _walkManager->getZ(x, y);
		m_node->setZ(z);
		//std::cout << " z = " << z << "\n";
	}
//    if (_zFunc) {
//        auto z = _zFunc(x, y).cast<float>();
//        m_node->setZ(z);
//    }
//    if (_scaleFunc) {
//        auto scale = _zFunc(x, y).cast<float>();
//        m_node->setScale(scale);
//    }
//
//
}

void CustomSierra2DController::update(double dt) {
    _callback(dt);
}

void PlayerSierra2DController::update(double dt) {
	if (!_enableControls) {
		return;
	}
	auto leftPressed = glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS;
	auto rightPressed = glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS;
	auto upPressed = glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS;
	auto downPressed = glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS;
	auto anyPressed = (leftPressed || rightPressed || upPressed || downPressed);

	if (leftPressed)
		_lookingLeft = true;

	if (rightPressed)
		_lookingLeft = false;

	if (leftPressed || rightPressed) {
		_direction = leftPressed ? "e" : "w";
	} else {
		if (upPressed)
			_direction = "n";
		else if (downPressed)
			_direction = "s";
	}




	float dx = (leftPressed || rightPressed ? 1.f : 0.f) * _speed * dt;
	float dy = (upPressed ? 1.f : (downPressed ? -1.f : 0.f)) * _speed * dt;

	if (anyPressed) {
		m_node->setFlipX(_lookingLeft);
	}
	float dir_x = _lookingLeft ? -1.f : 1.f;
	float dir_y = (dy > 0) ? 1.f : -1.f;
	auto pos = m_node->getWorldPosition();
	if (dx > 0) {
		// check horizontal
		glm::vec3 rayOrigin(pos.x + (_lookingLeft ? -_halfWidth : _halfWidth), pos.y, 0.f);
		float rayLength = dx * (_lookingLeft ? -1.f : 1.f);
		RayCastHit hit = m_collisionEngine->rayCast(rayOrigin, Direction::X, rayLength, 2, m_node);
		if (hit.collide) {
			dx = hit.length - _skinWidth;
		}
	}

	if (dy != 0.f) {
		glm::vec3 rayOriginLeft(pos.x + dx * dir_x - _halfWidth, pos.y, 0.f);
		glm::vec3 rayOriginRight(pos.x + dx * dir_x + _halfWidth, pos.y, 0.f);
		auto hit_left = m_collisionEngine->rayCast(rayOriginLeft, Direction::Y, dy, 2, m_node);
		if (hit_left.collide) {
			dy = dir_y * (hit_left.length - _skinWidth);
		}
		auto hit_right = m_collisionEngine->rayCast(rayOriginRight, Direction::Y,  dy, 2, m_node);
		if (hit_right.collide) {
			dy = dir_y * (hit_right.length - _skinWidth);
		}



	}

	m_node->move(glm::vec3(dx,dy,0));

    // update z
    Sierra2DController::update(dt);

    std::string dir(_direction);
    if (dir == "w") {
        dir = "e";
    }
    //m_animatedRenderer->setVersion(dir);
	if (anyPressed) {
		m_animatedRenderer->setAnimation(_walkAnimation + "_" + dir);
	} else {
		m_animatedRenderer->setAnimation(_idleAnimation + "_" + dir);
	}




}