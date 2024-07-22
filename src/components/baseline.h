#pragma once

#include "../component.h"
#include "../runners/walkarea.h"
#include "../shapes/polyline.h"

// a walkinfo can have
// - an area (or path) where characters are not allowed to walk
// - a baseline such that any character walking below it appears in front of the object.
class Baseline : public Component {
public:
	Baseline(const std::shared_ptr<PolyLine> polyline, const pybind11::kwargs& args);
	~Baseline() override;
	void start() override;
	PolyLine* getPolyline();
	float getZ() const;
	void setZ(float);
private:
	WalkManager* _walkArea;
	std::shared_ptr<PolyLine> _poly;
	float _z;
};