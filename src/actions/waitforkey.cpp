#include "waitforkey.h"
#include "../pyhelper.h"
#include <GLFW/glfw3.h>

WaitForKey::WaitForKey() : Action(), KeyboardListener(), _complete(false) {

	std::cout << "pippo\n";
}

void WaitForKey::add(int key, const pybind11::kwargs &args) {
	auto func = py_get_dict<pybind11::function>(args, "func", pybind11::function());
	m_functions[KeyboardEvent{key, GLFW_PRESS, 0}] =func;
}


int WaitForKey::keyCallback(GLFWwindow * w, int key, int scancode, int action, int mods) {
	if (_status != 1) return 0;
	auto it = m_functions.find(KeyboardEvent{key, action, mods});
	if (it != m_functions.end()) {
		if (it->second) {
		    it->second();
		}
		_complete = true;
		_status = 0;
		//Engine::instance().getRoom()->addCallback(it->second);

	}
	return 0;
}

int WaitForKey::process(double) {
	return _complete ? 0 : 1;
}
