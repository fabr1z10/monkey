#pragma once

#include <pybind11/pybind11.h>
#include "../component.h"
#include "../mouselistener.h"
#include "../camera.h"

class Camera;

// cursor component make the target follow the mouse position
class Cursor : public Component, public MouseListener {
public:
	Cursor (const pybind11::kwargs&);


	void start() override ;
	void update(double) override {}
	void cursorPosCallback(GLFWwindow*, double, double) override;
	void mouseButtonCallback(GLFWwindow*, int, int, int) override {}
	//virtual void ScrollCallback(GLFWwindow*, double, double) override {}
	//void setActive(bool) override;
	using ParentClass = Cursor;
private:
	//bool m_active;
	int _camId;
	OrthoCamera* _cam;
};