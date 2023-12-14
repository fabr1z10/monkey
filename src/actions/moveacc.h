#pragma once

#include "nodeaction.h"
#include <glm/glm.hpp>

class MoveAccelerated : public NodeAction {
public:
	explicit MoveAccelerated(int id, glm::vec3 velocity, glm::vec3 acceleration, float timeOut);
	int process(double) override;

private:
	float m_time;
	float m_timeOut;
	//glm::vec3 m_endPoint;
	glm::vec3 m_initialVelocity;
	glm::vec3 m_velocity;
	glm::vec3 m_acceleration;
};
