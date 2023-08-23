#include "hotspot.h"
#include "../engine.h"
#include "../pyhelper.h"
#include "../models/modelmake.h"
#include <iostream>
#include "../runners/hotspotmanager.h"

using namespace pybind11::literals; // to bring in the `_a` literal

HotSpot::HotSpot(std::shared_ptr<Shape> shape, const pybind11::kwargs& args) : Component(), _shape(shape) {
	_batchId = py_get_dict<std::string>(args, "batch", "");

}

HotSpot::~HotSpot() noexcept {
	Engine::instance().getRoom()->getRunner<HotSpotManager>()->remove(this);
}

void HotSpot::start() {

	auto& engine = Engine::instance();
	auto collDebug = py_get<bool>(engine.getConfig(), "debug_collision", false);
	if (collDebug && !_batchId.empty()) {
		auto model = ModelMaker::makeModel(_shape, glm::vec4(1.0f, 1.f, 1.f, 1.f));
		m_node->setModel(model, pybind11::dict("batch"_a = _batchId));


	}
	Engine::instance().getRoom()->getRunner<HotSpotManager>()->add(this);
}

void HotSpot::onEnter() {
	std::cout << "entering ...\n";
	if (_onEnter) {
		onEnter();
	}

}

void HotSpot::onLeave() {
	std::cout << "leaving ...\n";
}

void HotSpot::onClick(glm::vec2 pos, int button, int action) {
	std::cout << "clicking\n";
	if (_onClick) {
		_onClick(pos,button,action); //(pos, button, action);
	}
}