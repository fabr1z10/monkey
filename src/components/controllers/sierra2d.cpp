#include "sierra2d.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include "../../node.h"
#include "../../error.h"
#include "../../engine.h"
#include "../../pyhelper.h"


extern GLFWwindow* window;

Sierra2DController::Sierra2DController(const pybind11::kwargs &args) : Component() {
    //_yFront = py_get_dict<float>(args, "y_front");
    //_yBack = py_get_dict<float>(args, "y_back");
    _zFunc = py_get_dict<pybind11::function>(args, "z_func", pybind11::function());
    _scaleFunc = py_get_dict<pybind11::function>(args, "scale_func", pybind11::function());

    //_a = 2.f / (_yFront - _yBack);
    //_b = (_yBack + _yFront) / (_yBack - _yFront);
}

PlayerSierra2DController::PlayerSierra2DController(const pybind11::kwargs &args) : Sierra2DController(args), _enableControls(true) {
	_halfWidth = py_get_dict<float>(args, "half_width");
	_skinWidth = py_get_dict<float>(args, "skinWidth", .015f);
	_idleAnimation = py_get_dict<std::string>(args, "idle", "idle");
	_walkAnimation = py_get_dict<std::string>(args, "walk", "walk");
	_dir = py_get_dict<std::string>(args, "dir", "e");
}

void PlayerSierra2DController::setAnim(const std::string &idle, const std::string &walk) {
	_idleAnimation = idle;
	_walkAnimation = walk;
}


void PlayerSierra2DController::enable(bool value) {
	_enableControls = value;
}

void PlayerSierra2DController::start() {
	//_dir = 'e';
	_lookingLeft = false;
	m_animatedRenderer = m_node->getComponent<Renderer>();

	auto& engine = Engine::instance();
	auto room = engine.getRoom();
	m_collisionEngine = room->getRunner<ICollisionEngine>();
	M_Assert(m_collisionEngine != nullptr, "The component PlayerSierra2DController requires a collision engine.");
}

void Sierra2DController::update(double) {

    auto currentPos = m_node->getWorldPosition();
    if (currentPos != _previousPosition) {
        updateZ(currentPos.x, currentPos.y);
        _previousPosition = currentPos;
    }

}



void Sierra2DController::updateZ(float x, float y) {
    if (_zFunc) {
        auto z = _zFunc(x, y).cast<float>();
        m_node->setZ(z);
    }
    if (_scaleFunc) {
        auto scale = _zFunc(x, y).cast<float>();
        m_node->setScale(scale);
    }


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
		_dir = "e";
	} else {
		if (upPressed)
			_dir = "n";
		else if (downPressed)
			_dir = "s";
	}




	float dx = (leftPressed || rightPressed ? 1.f : 0.f);
	float dy = upPressed ? 1.f : (downPressed ? -1.f : 0.f);

	m_node->setFlipX(_lookingLeft);
	float dir_x = _lookingLeft ? -1.f : 1.f;
	float dir_y = (dy > 0) ? 1.f : -1.f;
	auto pos = m_node->getWorldPosition();
	if (dx > 0) {
		// check horizontal
		glm::vec3 rayOrigin(pos.x + (_lookingLeft ? -_halfWidth : _halfWidth), pos.y, 0.f);
		float rayLength = dx * (_lookingLeft ? -1.f : 1.f);
		RayCastHit hit = m_collisionEngine->rayCastX(rayOrigin, rayLength, 2, m_node);
		if (hit.collide) {
			dx = hit.length - _skinWidth;
		}
	}

	if (dy != 0.f) {
		glm::vec3 rayOriginLeft(pos.x + dx * dir_x - _halfWidth, pos.y, 0.f);
		glm::vec3 rayOriginRight(pos.x + dx * dir_x + _halfWidth, pos.y, 0.f);
		auto hit_left = m_collisionEngine->rayCastY(rayOriginLeft, dy, 2, m_node);
		if (hit_left.collide) {
			dy = dir_y * (hit_left.length - _skinWidth);
		}
		auto hit_right = m_collisionEngine->rayCastY(rayOriginRight, dy, 2, m_node);
		if (hit_right.collide) {
			dy = dir_y * (hit_right.length - _skinWidth);
		}



	}

	m_node->move(glm::vec3(dx,dy,0));

    // update z
    Sierra2DController::update(dt);

    std::string dir(_dir);
    if (dir == "w") {
        dir = "e";
    }
	if (anyPressed) {
		m_animatedRenderer->setAnimation(_walkAnimation + "_" + dir);
	} else {
		m_animatedRenderer->setAnimation(_idleAnimation + "_" + dir);
	}




}