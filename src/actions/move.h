#pragma once

#include "nodeaction.h"
#include <glm/glm.hpp>

class Move : public NodeAction {
public:
	explicit Move(const pybind11::kwargs&);
	int process(double) override;

private:
	int m_next;
	int m_tickCount;
	std::vector<int> m_ticks;
	std::vector<glm::vec3> m_positions;
};

class MoveBy : public NodeAction {
public:
	explicit MoveBy(const pybind11::kwargs&);
	int process(double) override;
	void start() override;

private:
	int m_id;
	glm::vec3 m_endPoint;
	float m_speed;
	float m_distance;
	float m_distanceTraveled;
	glm::vec3 m_delta;
	glm::vec3 m_unitVec;
};
