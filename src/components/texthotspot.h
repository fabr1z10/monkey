#pragma once

#include "hotspot.h"

class TextHotSpot : public HotSpot {
public:
	TextHotSpot(const pybind11::kwargs& args);

	void start() override;

	std::type_index getType() override;

};