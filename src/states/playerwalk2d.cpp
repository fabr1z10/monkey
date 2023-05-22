#include "playerwalk2d.h"
#include "../node.h"
#include <GLFW/glfw3.h>

extern GLFWwindow* window;



void PlayerWalk2D::control() {
	if (m_controller->grounded() && !m_wasGroudned) {

	}
	m_wasGroudned = m_controller->grounded();

	m_left = glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS;
	m_right = glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS;
	m_up = glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS;
}

void PlayerWalk2D::keyCallback(int key) {

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
