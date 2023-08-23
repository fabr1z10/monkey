#pragma once

#include <unordered_set>
#include "../runner.h"
#include "../mouselistener.h"
#include "../components/hotspot.h"

class OrthoCamera;

class HotSpotManager : public Runner, public MouseListener {
public:
	HotSpotManager() = default;
	virtual ~HotSpotManager() {}
	void cursorPosCallback(GLFWwindow*, double, double) override;
	void mouseButtonCallback(GLFWwindow*, int, int, int) override;

	void start() override;
	void update(double) override {}
	void add(HotSpot*);
	void remove(HotSpot*);
private:
	glm::vec2 _lastPosition;
	void updateCurrent(HotSpot*);
	OrthoCamera* m_camera;
	std::unordered_set<HotSpot*> m_hotspot;
	HotSpot* m_previous;
	glm::vec2 _deviceSize;
};