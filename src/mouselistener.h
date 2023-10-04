#pragma once

#include <functional>


struct GLFWwindow;

class MouseListener {
public:
	MouseListener();
	virtual~ MouseListener();
	virtual void cursorPosCallback(GLFWwindow*, double, double) = 0;
	virtual void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) = 0;
};


class MouseListenerFunc : public MouseListener {
public:
	MouseListenerFunc(std::function<void(GLFWwindow*, double, double)> onMove, std::function<void(GLFWwindow*, int, int, int)> onClick);
	void cursorPosCallback(GLFWwindow*, double, double) override;
	void mouseButtonCallback(GLFWwindow*, int, int, int) override;

private:
	std::function<void(GLFWwindow*, double, double)> _onMove;
	std::function<void(GLFWwindow*, int, int, int)> _onClick;
};