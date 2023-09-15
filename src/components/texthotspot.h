#pragma once

#include "hotspot.h"

class TextHotSpot : public PyHotSpot {
public:
	TextHotSpot(const pybind11::kwargs& args);

	void start() override;

	std::type_index getType() override;

};