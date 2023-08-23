#pragma once

#include "nodeaction.h"
#include <glm/glm.hpp>

class Walk : public NodeAction {
public:
	Walk(const pybind11::kwargs&);
	void start() override;
	int run(double) override{}
private:
	glm::vec2 _target;
};