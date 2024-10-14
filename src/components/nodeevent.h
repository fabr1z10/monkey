#pragma once

#include "../component.h"
#include "../camera.h"
#include <pybind11/pybind11.h>



// perform callback when node
class NodeEvent : public Component {
public:
	NodeEvent(float width, float height, const pybind11::kwargs& args);
	void update(double) override;
	void start() override {}
	void addEventX(float, int, pybind11::function, const pybind11::kwargs& kwargs);
	void addEventY(float, int, pybind11::function, const pybind11::kwargs& kwargs);
protected:
	struct Event {
		// 1 = event fires if I am moving towards positive values
		// 0 = event fires always
		// -1 = event fires if I am moving towards negative values
		int direction;
		int iOther0, iOther1;
		pybind11::function callback;
		pybind11::kwargs kwargs;

	};
	glm::vec2 _size;
	glm::ivec2 _previous;
	std::unordered_map<int, std::vector<Event>> _eventX;
	std::unordered_map<int, std::vector<Event>> _eventY;
};