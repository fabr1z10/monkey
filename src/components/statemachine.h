#pragma once

#include "../component.h"
#include <memory>
#include <unordered_map>
#include <pybind11/pytypes.h>
#include "../keylistener.h"

class StateMachine;

class State {
public:
	State(const std::string& id, const pybind11::kwargs&);
	virtual ~State() = default;
	std::string getId() const;
	virtual void keyCallback(int);
	virtual void init(const pybind11::kwargs& args);
	virtual void run(double) {}
	virtual void end();
	virtual void setParent(StateMachine*);
protected:
	pybind11::function m_script;
	std::string m_id;
	StateMachine* m_sm;
	//long m_scriptId;
	bool m_current;
	std::unordered_map<int, pybind11::function> m_keyCallbacks;
	bool m_overrideCollision;
};

inline void State::setParent(StateMachine * sm) {
	m_sm = sm;
}

inline std::string State::getId() const {
	return m_id;
}

//!  A state machine.
/*!
 A state machine is a component that allows an entity to have a different
 behavior based on what the current state is. A state machine has a set of
 states, of which only one is active at any given time. It provides methods
 to get the current state, and change the state to a new one.
 */
class StateMachine : public Component, public KeyboardListener {
public:
	StateMachine() : Component(), m_currentState(nullptr) {}
	void start () override;
	void update(double) override;
	std::shared_ptr<State> getState() const;
	void keyCallback(GLFWwindow*, int key, int scancode, int action, int mods) override;
	void setState(const std::string&, const pybind11::kwargs& kwargs = pybind11::kwargs());
	void addState(std::shared_ptr<State> state);
	void setInitialState(const std::string& id, const pybind11::kwargs& args);
protected:
	std::shared_ptr<State> m_currentState;
	std::string m_initialState;
	pybind11::kwargs m_args;
	std::unordered_map<std::string, std::shared_ptr<State>> m_states;
};

inline void StateMachine::addState(std::shared_ptr<State> state) {
	m_states.insert(std::make_pair(state->getId(), state));
}

inline std::shared_ptr<State> StateMachine::getState() const {
	return m_currentState;
}

inline void StateMachine::setInitialState(const std::string &id, const pybind11::kwargs& kwargs) {
	m_initialState = id;
	m_args = kwargs;
}


