#pragma once

#include "nodeaction.h"
#include <glm/glm.hpp>

class MoveAccelerated : public NodeAction {
public:
	explicit MoveAccelerated(const pybind11::kwargs&);
	int run(double) override;

private:
	float m_time;
	float m_timeOut;
	//glm::vec3 m_endPoint;
	glm::vec3 m_initialVelocity;
	glm::vec3 m_velocity;
	glm::vec3 m_acceleration;
};
