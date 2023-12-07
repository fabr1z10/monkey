#include "sierra2d.h"
#include <GLFW/glfw3.h>
#include "../../node.h"
#include "../../engine.h"
#include "../../pyhelper.h"

extern GLFWwindow* window;

Sierra2DController::Sierra2DController(const pybind11::kwargs &args) : Component(), _enableControls(true) {
	_halfWidth = py_get_dict<float>(args, "half_width");
	_skinWidth = py_get_dict<float>(args, "skinWidth", .015f);
	_idleAnimation = py_get_dict<std::string>(args, "idle", "idle");
	_walkAnimation = py_get_dict<std::string>(args, "walk", "walk");
}

void Sierra2DController::setAnim(const std::string &idle, const std::string &walk) {
	_idleAnimation = idle;
	_walkAnimation = walk;
}


void Sierra2DController::enable(bool value) {
	_enableControls = value;
}

void Sierra2DController::start() {
	_dir = 'e';
	_lookingLeft = false;
	m_animatedRenderer = m_node->getComponent<Renderer>();

	auto& engine = Engine::instance();
	auto room = engine.getRoom();
	m_collisionEngine = room->getRunner<ICollisionEngine>();
}

void Sierra2DController::update(double) {
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



	if (anyPressed) {
		m_animatedRenderer->setAnimation(_walkAnimation + "_" + _dir);
	} else {
		m_animatedRenderer->setAnimation(_idleAnimation + "_" + _dir);
	}




}