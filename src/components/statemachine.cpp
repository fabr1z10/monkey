#include "statemachine.h"
#include "../error.h"
#include "../engine.h"
//#include "../runners/scheduler.h"
#include "../pyhelper.h"
#include "collider.h"

void State::setParent(StateMachine * sm, const pybind11::kwargs& args) {
    m_sm = sm;
    m_script = py_get_dict<pybind11::function>(args, "script", pybind11::function());
	if (args.contains("keys")) {
		auto keys = args["keys"].cast<pybind11::dict>();
		for (const auto& key : keys) {
			auto keyId = key.first.cast<int>();
			auto callback = key.second.cast<pybind11::function>();
			m_keyCallbacks.insert(std::make_pair(keyId, callback));
		}
	}
}

void State::keyCallback(int key) {
	auto it = m_keyCallbacks.find(key);
	if (it != m_keyCallbacks.end()) {
		it->second();
	}

}

void StateMachine::keyCallback(GLFWwindow *, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		if (m_currentState != nullptr) m_currentState->keyCallback(key);
	}
}


void State::init(const pybind11::kwargs& args) {
	m_current = true;
//	if (m_script) {
//		m_scriptId = m_script(m_sm->getNode()->getId()).cast<long>();
//	}
}

void State::end() {
	m_current = false;
//	if (m_scriptId != -1) {
//		Engine::instance().getRoom()->getRunner<Scheduler>()->kill(m_scriptId);
//	}

}

void StateMachine::start() {
	for (auto& s : m_states) {
		s.second->start();
	}
	if (!m_initialState.empty()) {
		setState(m_initialState, m_args);
	}
}

void StateMachine::addState(const std::string& id, std::shared_ptr<State> state, const pybind11::kwargs& args) {
    state->setParent(this, args);
    m_states.insert(std::make_pair(id, state));
}


void StateMachine::setState(const std::string & state, const pybind11::kwargs& args) {
	auto it = m_states.find(state);
	if (it == m_states.end()) {
		GLIB_FAIL("Don't know state: " + state);
	}
	if (m_currentState) {
		m_currentState->end();
	}
	it->second->init(args);
	m_currentState = it->second;
}

void StateMachine::update(double dt) {
	if (m_currentState != nullptr) {
		m_currentState->run(dt);
	}
}