#include "keyboard.h"
#include "../engine.h"


int Keyboard::keyCallback(GLFWwindow * w, int key, int scancode, int action, int mods) {
    if (_state != NodeState::ACTIVE) {
        // ignore key
        return 0;
    }
	auto it = m_functions.find(KeyboardEvent{key, action, mods});
	if (it != m_functions.end()) {
		it->second();
		return 1;
		//Engine::instance().getRoom()->addCallback(it->second);

	}
	if (_fallbackFunction) {
	    _fallbackFunction(key, action, mods);
	    return 1;
	}
	return 0;
}

void Keyboard::addFunction(int key, int action, int mods, pybind11::function f) {
	m_functions[KeyboardEvent{key, action, mods}] = f;
}

void Keyboard::addFallbackFunction(pybind11::function f) {
    _fallbackFunction = f;
}