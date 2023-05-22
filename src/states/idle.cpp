#include "idle.h"
#include "../node.h"
#include "../pyhelper.h"


void Idle::setParent(StateMachine* sm, const pybind11::kwargs& args) {
    State::setParent(sm, args);
    m_animation = py_get_dict<std::string>(args, "anim");
	m_exitState = py_get_dict<std::string>(args, "exit_state", "");
	m_exitOnComplete = py_get_dict<bool>(args, "exit_on_complete", false);
	m_timeOut = py_get_dict<float>(args, "timeout", 0.f);
    auto node = sm->getNode();
    m_spriteRenderer = dynamic_cast<SpriteRenderer*>(node->getComponent<Renderer>());

}

void Idle::init(const pybind11::kwargs& args) {
	State::init(args);
	auto anim = py_get_dict<std::string>(args, "anim", m_animation);
	m_spriteRenderer->setAnimation(anim);
	m_time = 0.f;
}



void Idle::run(double dt) {
	if (!m_exitState.empty()) {
		if (m_exitOnComplete) {
			if (m_spriteRenderer->isComplete()) {
				m_sm->setState(m_exitState);
			}
		} else {
			m_time += static_cast<float>(dt);
			if (m_time >= m_timeOut) {
				m_sm->setState(m_exitState);
			}
		}
	}
}