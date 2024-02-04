#include "../keylistener.h"
#include "../component.h"
#include <unordered_map>
#include <pybind11/pybind11.h>


class Keyboard : public Component, public KeyboardListener {
public:
	int keyCallback(GLFWwindow*, int key, int scancode, int action, int mods) override;
	void addFunction(int, int, int, pybind11::function f);
private:
	std::unordered_map<KeyboardEvent, pybind11::function> m_functions;
};