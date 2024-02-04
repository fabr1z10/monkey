#pragma once

#include "../keylistener.h"
#include "../runners/scheduler.h"

class WaitForKey : public Action, public KeyboardListener {
public:
	WaitForKey();

	void add(int, const pybind11::kwargs& args);
	int keyCallback(GLFWwindow*, int key, int scancode, int action, int mods) override;
	int process(double);
private:
	std::unordered_map<KeyboardEvent, pybind11::function> m_functions;
	bool _complete;
};