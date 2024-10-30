#pragma once

#include "nodeaction.h"
#include <glm/glm.hpp>

class Move : public NodeAction {
public:
	explicit Move(int id, glm::vec2 pos, float speed);
	int process(double) override;
	void start() override;
	void reset() override;
private:
	float _time;
	float _speed;
	glm::vec2 _targetPos;
	glm::vec2 _dir;
	float _length;
	float _distanceTraversed;
};

class MoveBy : public NodeAction {
public:
	explicit MoveBy(int id, glm::vec2 delta, float time, float speed);
	int process(double) override;
	void start() override;
	void reset() override;
private:
	int m_id;
	glm::vec3 m_endPoint;
	float m_speed;
	float m_distance;
	float m_distanceTraveled;
	glm::vec3 m_delta;
	glm::vec3 m_unitVec;
};


class MoveAccelerated : public NodeAction {
public:
    MoveAccelerated(int id, glm::vec3 initialVelocity, glm::vec3 acceleration, const pybind11::kwargs&);
    int process(double) override;

private:
    glm::vec3 _velocity;
    glm::vec3 _acceleration;
    float _yMin;

};