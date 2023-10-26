#include "cursor.h"
#include "../engine.h"
#include "../pyhelper.h"
#include <iostream>

Cursor::Cursor(const pybind11::kwargs & kwargs) : Component(), MouseListener() {
	_camId = py_get_dict<int>(kwargs, "cam");
}

void Cursor::start() {
	_cam = dynamic_cast<OrthoCamera*>(Engine::instance().getRoom()->getCamera(_camId));
}

void Cursor::cursorPosCallback(GLFWwindow*, double x, double y) {
	// first check if we are within the device coordinates
	auto wvp = Engine::instance().getWindowViewport();
	if (x < wvp[0] || x > wvp[0] + wvp[2] || y < wvp[1] || y > wvp[1] + wvp[3]) {
		return;
	}
	auto _deviceSize = Engine::instance().getDeviceSize();

	float winHeight = Engine::instance().getWindowSize().y;
	// convert window coordinates into device coordinates
	float dev_x = (x - wvp[0]) * (_deviceSize.x / wvp[2]);
	float dev_y = (winHeight - y - wvp[1]) * (_deviceSize.y / wvp[3]);

	auto cvp = _cam->getViewport();
	if (dev_x < cvp[0] || dev_x > cvp[0] + cvp[2] || dev_y < cvp[1] || dev_y > cvp[1] + cvp[3]) {
		return;
	}


	glm::vec2 wp = _cam->getWorldCooridnates(dev_x, dev_y);
	std::cout << x << ", " << y << ", --> " << wp.x << ", " << wp.y << "\n";
	m_node->setPosition(wp.x, wp.y, 1.f);




}