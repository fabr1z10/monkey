#include "hotspot.h"
#include "../engine.h"
#include "../pyhelper.h"
#include "../models/modelmake.h"
#include <iostream>
#include "../runners/hotspotmanager.h"

using namespace pybind11::literals; // to bring in the `_a` literal

HotSpot::HotSpot(std::shared_ptr<Shape> shape, const pybind11::kwargs& args) : Component(), _shape(shape) {
	_batchId = py_get_dict<std::string>(args, "batch");
	_priority = py_get_dict<int>(args, "priority", 0);
}

PyHotSpot::PyHotSpot(std::shared_ptr<Shape> shape, const pybind11::kwargs &args) : HotSpot(shape, args) {}


HotSpot::~HotSpot() noexcept {
	Engine::instance().getRoom()->getRunner<HotSpotManager>()->remove(this);
}

void HotSpot::start() {

	auto& engine = Engine::instance();
	auto collDebug = py_get<bool>(engine.getConfig(), "debug_collision", false);
	if (collDebug && !_batchId.empty()) {
		auto debugNode = std::make_shared<Node>();

		auto model = ModelMaker::makeModel(_shape, glm::vec4(1.0f, 1.f, 1.f, 1.f), FillType::OUTLINE);
		debugNode->setModel(model, pybind11::dict("batch"_a = _batchId));
		m_node->add(debugNode);

	}
	Engine::instance().getRoom()->getRunner<HotSpotManager>()->add(this);
}

void PyHotSpot::onEnter() {
	std::cout << "entering ...\n";
	if (_onEnter) {
		_onEnter(m_node);
	}

}

void PyHotSpot::onLeave() {
	std::cout << "leaving ...\n";
	if (_onLeave) {
		_onLeave(m_node);
	}

}

void PyHotSpot::onClick(glm::vec2 pos, int button, int action) {
	std::cout << "clicking\n";
	if (_onClick) {
		_onClick(m_node, pos,button,action); //(pos, button, action);
	}
}