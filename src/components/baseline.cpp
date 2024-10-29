#include "baseline.h"
#include "../engine.h"
#include "../runners/walkarea.h"


Baseline::Baseline(std::shared_ptr<shapes::PolyLine> polyline, const pybind11::kwargs& args) : Component(args), _poly(polyline){

}

void Baseline::start() {
	auto& engine = Engine::instance();
	auto room = engine.getRoom();
	_walkArea = room->getRunner<WalkManager>();
	_walkArea->addBaseLine(this);
	//_walkArea->reco//mputeBaselines();
	//walkArea->addDynamic()
	//m_engine = Engine::get().GetRunner<ICollisionEngine>();
}

Baseline::~Baseline() noexcept {
	_walkArea->rmBaseline(this);
	//_walkArea->recomputeBaselines();
}

shapes::PolyLine * Baseline::getPolyline() {
	return _poly.get();
}
float Baseline::getZ() const {
	return _z;
}
void Baseline::setZ(float value) {
	_z = value;
}