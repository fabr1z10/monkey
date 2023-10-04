#include "mouselistener.h"
#include "engine.h"

MouseListener::MouseListener() {
	Engine::instance().registerToMouseEvent(this);

}

MouseListener::~MouseListener() {
	Engine::instance().unregisterToMouseEvent(this);

}

MouseListenerFunc::MouseListenerFunc(std::function<void(GLFWwindow*, double, double)> onMove, std::function<void(GLFWwindow*, int, int, int)> onClick) :
	MouseListener(), _onMove(onMove), _onClick(onClick) {
}

void MouseListenerFunc::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
	_onClick(window, button, action, mods);
}

void MouseListenerFunc::cursorPosCallback(GLFWwindow * window, double x, double y) {
	_onMove(window, x, y);

}