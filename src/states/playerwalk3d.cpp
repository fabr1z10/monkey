#include "playerwalk3d.h"
#include "../node.h"
#include <GLFW/glfw3.h>

extern GLFWwindow* window;

PlayerWalk3D::PlayerWalk3D(const pybind11::kwargs &kwargs) : Walk3D(kwargs) {
	/// default way to jmp is via arrow up key
	_keyJump = py_get_dict<int>(kwargs, "key_jmp", GLFW_KEY_LEFT_CONTROL);
}

void PlayerWalk3D::control() {
//	if (m_controller->grounded() && !m_wasGroudned) {
//
//	}
//	m_wasGroudned = m_controller->grounded();

	_keys.x = glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS ? 1 :
			  (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS ? -1 : 0);
	_keys.z = glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS ? 1 :
			  (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS ? -1 : 0);
	_keys.jump = glfwGetKey(window, _keyJump) == GLFW_PRESS;
}

void PlayerWalk3D::keyCallback(int key) {

	switch (key) {
		case GLFW_KEY_LEFT:
			m_sm->getNode()->setFlipX(true);
			break;
		case GLFW_KEY_RIGHT:
			m_sm->getNode()->setFlipX(false);
			break;
	}
	State::keyCallback(key);
}
