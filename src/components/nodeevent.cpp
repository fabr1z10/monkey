#include "nodeevent.h"
#include "../node.h"

NodeEvent::NodeEvent(float width, float height, const pybind11::kwargs &args) : Component(args), _size(width, height) {}

void NodeEvent::update(double) {
	auto pos = m_node->getWorldPosition();

	int ix = static_cast<int>(pos.x / _size.x);
	int iy = static_cast<int>(pos.y / _size.y);

	// check event at this position
	if (ix != _previous.x) {
		auto it = _eventX.find(ix);

		if (it != _eventX.end()) {
			auto dir = (ix > _previous.x ? 1 : -1);
			for (const auto& event : it->second) {
				if (event.direction == dir || event.direction == 0) {
					event.callback(**event.kwargs);
				}
			}
		}
	}

	// check event at this position
	if (iy != _previous.y) {
		auto it = _eventY.find(iy);

		if (it != _eventY.end()) {
			auto dir = (iy > _previous.y ? 1 : -1);
			for (const auto& event : it->second) {
				if (event.direction == dir || event.direction == 0) {
					event.callback(**event.kwargs);
				}
			}
		}
	}


	_previous = glm::ivec2(ix, iy);

}

void NodeEvent::addEventX(float x, int direction, pybind11::function f, const pybind11::kwargs& args) {
	_eventX[static_cast<int>(x / _size.x)].push_back({direction, 0, 0, f, args});
}
void NodeEvent::addEventY(float y, int direction, pybind11::function f, const pybind11::kwargs& args) {
	_eventX[static_cast<int>(y / _size.y)].push_back({direction, 0, 0, f, args});
}