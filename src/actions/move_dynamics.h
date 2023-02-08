#pragma once

#include "nodeaction.h"
#include "../components/controller2d.h"
#include "../components/dynamics.h"
#include <glm/glm.hpp>

class MoveDynamics : public NodeAction {
public:
	explicit MoveDynamics(const pybind11::kwargs&);
	void start() override;

	int run(double) override;

private:
	glm::vec3 m_initialVelocity;
	glm::vec3 m_acceleration;
	Controller2D* m_controller;
	Dynamics * m_dyn;
};
