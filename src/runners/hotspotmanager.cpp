#include "hotspotmanager.h"
#include "../engine.h"
#include "../room.h"
#include <iostream>


void HotSpotManager::start() {
	m_camera = dynamic_cast<OrthoCamera*>(Engine::instance().getRoom()->getCamera(0));
	assert(m_camera != nullptr);
	m_previous = nullptr;
	_deviceSize = Engine::instance().getDeviceSize();
}

void HotSpotManager::updateCurrent(HotSpot* hotspot) {
	if (m_previous != hotspot) {
		if (m_previous != nullptr) {
			m_previous->onLeave();
		}
		m_previous = hotspot;
		if (hotspot != nullptr) {
			hotspot->onEnter();
		}
	}

}

void HotSpotManager::cursorPosCallback(GLFWwindow *, double x, double y) {
	// first check if we are within the device coordinates
	auto wvp = Engine::instance().getWindowViewport();

	if (x < wvp[0] || x > wvp[0] + wvp[2] || y < wvp[1] || y > wvp[1] + wvp[3]) {
		// do nothing except
		std::cout << "outside device\n";
		updateCurrent(nullptr);
		return;
	}
	float winHeight = Engine::instance().getWindowSize().y;
	// convert window coordinates into device coordinates
	float dev_x = (x - wvp[0]) * (_deviceSize.x / wvp[2]);
	float dev_y = (winHeight - y - wvp[1]) * (_deviceSize.y / wvp[3]);
	auto cvp = m_camera->getViewport();
	if (dev_x < cvp[0] || dev_x > cvp[0] + cvp[2] || dev_y < cvp[1] || dev_y > cvp[1] + cvp[3]) {
		// outside camera viewport
		std::cout << "outside camera\n";
		updateCurrent(nullptr);
		return;
	}
	// convert device coordinates into camera coordinates
	auto wc = m_camera->getWorldCooridnates(dev_x, dev_y);
	//std::cout << x << ", " << y << " --- " << wc.x << ", " << wc.y << "\n";
	glm::vec3 P(wc.x, wc.y, 0.f);
	_lastPosition = glm::vec2(wc.x, wc.y);
	HotSpot* selected = nullptr;
	float zSelected = 0.f;
	for (const auto& hotspot : m_hotspot) {
		if (hotspot->getShape()->isInside(P)) {

			auto z = hotspot->getNode()->getWorldPosition().z;
			if (selected == nullptr || z > zSelected) {
				selected = hotspot;
				zSelected = z;
			}
		}
	}

	updateCurrent(selected);


}


void HotSpotManager::mouseButtonCallback(GLFWwindow * window, int button, int action, int mods) {
	if (m_previous != nullptr) {

		m_previous->onClick(_lastPosition, button, action);
	}
}

void HotSpotManager::add(HotSpot * h) {
	m_hotspot.insert(h);
}

void HotSpotManager::remove(HotSpot * h) {
	m_hotspot.erase(h);
}