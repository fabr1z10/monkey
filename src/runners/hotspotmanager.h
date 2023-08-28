#pragma once

#include <unordered_set>
#include "../runner.h"
#include "../mouselistener.h"
#include "../components/hotspot.h"

class OrthoCamera;

class HotSpotManager : public Runner, public MouseListener {
public:
	HotSpotManager();
	virtual ~HotSpotManager() {}
	void cursorPosCallback(GLFWwindow*, double, double) override;
	void mouseButtonCallback(GLFWwindow*, int, int, int) override;

	void start() override;
	void update(double) override {}
	void add(HotSpot*);
	void remove(HotSpot*);
private:
	struct CamData {
		OrthoCamera* cam;
		std::unordered_set<HotSpot*> hotspots;
	};
	glm::vec2 _lastPosition;
	void updateCurrent(HotSpot*);

	//std::unordered_set<HotSpot*> m_hotspot;

	std::unordered_map<int, CamData> _items;
	HotSpot* m_previous;
	glm::vec2 _deviceSize;
	int _camId;
};