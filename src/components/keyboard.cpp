#include "keyboard.h"

void Keyboard::keyCallback(GLFWwindow * w, int key, int scancode, int action, int mods) {
	auto it = m_functions.find(KeyboardEvent{key, action, mods});
	if (it != m_functions.end()) {
		it->second();
	}
}

void Keyboard::addFunction(int key, int action, int mods, pybind11::function f) {
	m_functions[KeyboardEvent{key, action, mods}] = f;
}