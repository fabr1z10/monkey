#include "top2d.h"
#include "../util.h"

extern GLFWwindow* window;


Top2D::Top2D(const pybind11::kwargs& kwargs) : State(),
	m_controller(nullptr), m_dynamics(nullptr), m_node(nullptr), m_animatedRenderer(nullptr)
{

	_maxSpeed = py_get_dict<float>(kwargs, "speed", 10.f);
	_accelerationTime = py_get_dict<float>(kwargs, "acc_time", 0.1f);
	_rotationSpeed =  py_get_dict<float>(kwargs, "rot_speed", 1.f);
	if (_accelerationTime == 0.f)
		_acceleration = 0.f;
	else
		_acceleration = _maxSpeed / _accelerationTime;
}

void Top2D::start() {
	auto node = m_sm->getNode();
	m_node = node;

	m_controller = node->getComponent<Controller>();
	assert(m_controller != nullptr);

	m_dynamics = node->getComponent<Dynamics>();
	assert(m_dynamics != nullptr);

	m_animatedRenderer = node->getComponent<Renderer>();
}

void Top2D::run(double dt) {
	auto dtf = static_cast<float>(dt);

	bool left = glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS;
	bool right =glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS;
	if (left) {
		m_node->rotate(-dt * _rotationSpeed, glm::vec3(0,0,1));
	} else if (right) {
		m_node->rotate(dt * _rotationSpeed, glm::vec3(0,0,1));
	}


}