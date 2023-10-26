#include "hotspotmanager.h"
#include "../engine.h"
#include "../room.h"
#include <iostream>

HotSpotManager::HotSpotManager() : Runner() {}

void HotSpotManager::start() {
	auto room = Engine::instance().getRoom();
	for (size_t i = 0; i <room->getCameraCount(); ++i) {
		auto cam = dynamic_cast<OrthoCamera*>(room->getCamera(i));
		if (cam != nullptr) {
			_items[i] = CamData{cam, std::unordered_set<HotSpot*>()};
		}
	}

	assert(!_items.empty());
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
		std::cout << "outside window " << _camId << "\n";
		updateCurrent(nullptr);
		return;
	}
	float winHeight = Engine::instance().getWindowSize().y;
	// convert window coordinates into device coordinates
	float dev_x = (x - wvp[0]) * (_deviceSize.x / wvp[2]);
	float dev_y = (winHeight - y - wvp[1]) * (_deviceSize.y / wvp[3]);

	HotSpot *selected = nullptr;
	for (const auto& [id, data] : _items) {
		auto cvp = data.cam->getViewport();
		if (dev_x < cvp[0] || dev_x > cvp[0] + cvp[2] || dev_y < cvp[1] || dev_y > cvp[1] + cvp[3]) {
			// outside camera viewport
			//std::cout << "outside camera " << id << "\n";
			continue;
		}

		// convert device coordinates into camera coordinates
		auto wc = data.cam->getWorldCooridnates(dev_x, dev_y);
		std::cout << x << ", " << y << " --- " << wc.x << ", " << wc.y << "\n";
		glm::vec3 P(wc.x, wc.y, 0.f);
		_lastPosition = glm::vec2(wc.x, wc.y);

		float zSelected = 0.f;
		int prioritySelected = 0;
		for (const auto &hotspot : data.hotspots) {
			// transorm world coords in local coords
			if (!hotspot->isActive()) {
				continue;
			}
			glm::vec3 lP = P - hotspot->getNode()->getWorldPosition();
			if (hotspot->getShape()->isInside(lP)) {

				auto z = hotspot->getNode()->getWorldPosition().z;
				int currentPriority = hotspot->getPriority();
				if (selected == nullptr || currentPriority > prioritySelected || (currentPriority == prioritySelected && z > zSelected)) {
					selected = hotspot;
					zSelected = z;
					prioritySelected = currentPriority;
				}
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
	auto room = Engine::instance().getRoom();
	auto camId = room->getBatch(h->getBatch())->getCameraId();
	_items.at(camId).hotspots.insert(h);
}

void HotSpotManager::remove(HotSpot * h) {
	auto room = Engine::instance().getRoom();
	auto camId = room->getBatch(h->getBatch())->getCameraId();
	_items.at(camId).hotspots.erase(h);
	if (m_previous == h) {
		m_previous = nullptr;
	}
}