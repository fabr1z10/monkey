#include "climb.h"
#include "../pyhelper.h"
#include "../node.h"
#include "../util.h"
#include <GLFW/glfw3.h>

extern GLFWwindow* window;


void Climb::setParent(StateMachine * sm, const pybind11::kwargs& kwargs) {
    State::setParent(sm, kwargs);
    m_speed = py_get_dict<float>(kwargs, "speed", 0.0f);
    m_animation = kwargs["anim"].cast<std::string>();
    m_animIdle = kwargs["anim_idle"].cast<std::string>();
    m_walkState = py_get_dict<std::string>(kwargs, "walk_state", "walk");
    m_maskUp = py_get_dict<int>(kwargs, "mask_up", 2);
    m_maskDown = py_get_dict<int>(kwargs, "mask_up", 2 | 32);
	m_node = sm->getNode();

}

void Climb::start() {
    auto node = m_sm->getNode();

    m_controller = dynamic_cast<Controller2D*>(node->getComponent<Controller>());
    assert(m_controller != nullptr);

    m_dynamics =node->getComponent<Dynamics>();
    assert(m_dynamics != nullptr);

    m_animatedRenderer = dynamic_cast<SpriteRenderer*>(node->getComponent<Renderer>());
}


void Climb::init(const pybind11::kwargs& args) {
	State::init(args);
	m_node->setFlipX(false);
	m_animatedRenderer->setAnimation(m_animIdle);
	m_dynamics->m_velocity = glm::vec3(0.f);
	m_mask = m_controller->getMask();
	m_controller->setMask(m_maskUp, m_maskDown);
	glfwGetKey(window, GLFW_KEY_DOWN);
}

void Climb::end() {
	State::end();
	m_controller->setMask(m_mask[0], m_mask[1]);
}

void Climb::run(double dt) {
	auto dtf = static_cast<float>(dt);

	auto left = glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS;
	auto right = glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS;
	auto up = glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS;
	auto down = glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS;

	if (m_controller->grounded() && down) {
		m_sm->setState(m_walkState);
	}
	m_dynamics->m_velocity = glm::vec3(0.f);

	if (left || right) {
		m_dynamics->m_velocity.x = m_speed;
	}

	if (up) {
		m_dynamics->m_velocity.y = m_speed;
	} else if (down) {
		m_dynamics->m_velocity.y = -m_speed;
	}

	if (left) {
		m_node->setFlipX(true);
	}
	if (right) {
		m_node->setFlipX(false);
	}


	m_animatedRenderer->setAnimation((up || down || left || right) ? m_animation : m_animIdle);

	auto delta = m_dynamics->m_velocity * dtf;

	m_controller->move(delta, false);

}