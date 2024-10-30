#include "move.h"
#include "../pyhelper.h"
#include "../node.h"
#include <glm/gtx/transform.hpp>

Move::Move(int id, glm::vec2 pos, float speed) : NodeAction(id), _targetPos(pos), _speed(speed) {


}

void Move::start() {
	NodeAction::start();
	reset();
}

void Move::reset() {
	auto startPosition = glm::vec2(m_node->getWorldPosition());
	_dir = (_targetPos - startPosition);
	if (_speed == 0.f) {
		m_node->setPosition(_targetPos.x, _targetPos.y);
		stop();
	} else {
		_length = glm::length(_dir);
		_dir = glm::normalize(_dir);
		_distanceTraversed = 0.f;
	}
}


int Move::process(double dt) {
	glm::vec2 delta = _speed * static_cast<float>(dt) * _dir;
	m_node->move(glm::vec3(delta, 0.f));
	_distanceTraversed += glm::length(delta);

	if (_distanceTraversed >= _length) {
		m_node->setPosition(_targetPos.x, _targetPos.y);//, _targetPos.z);
		return 0;

	}
	return 1;
}

MoveBy::MoveBy(int id, glm::vec2 delta, float time, float speed) : NodeAction(id) {

	m_distance = glm::length(delta);
	m_delta = glm::vec3(delta, 0.f);
	m_unitVec = glm::normalize(m_delta);
	if (time < 0) {
		m_speed = speed;
	} else {
		m_speed = m_distance / time;
	}

}

void MoveBy::start() {
	NodeAction::start();

}

void MoveBy::reset() {
	m_endPoint = m_node->getWorldPosition() + m_delta;
	m_distanceTraveled = 0.f;

}


int MoveBy::process(double dt) {
	auto dtf = static_cast<float>(dt);
	auto dist = m_speed * dtf;
	m_distanceTraveled += dist;
	auto delta = m_unitVec * dist;
	if (m_distanceTraveled >= m_distance) {
		m_node->setPosition(m_endPoint.x, m_endPoint.y, m_endPoint.z);
		return 0;
	}
	m_node->move(glm::translate(delta));

	return 1;
}

MoveAccelerated::MoveAccelerated(int id, glm::vec3 initialVelocity, glm::vec3 acceleration, const pybind11::kwargs& args) : NodeAction(id),
	_velocity(initialVelocity), _acceleration(acceleration) {
	_yMin = py_get_dict<float>(args, "y_min", -std::numeric_limits<float>::max());
}


int MoveAccelerated::process(double dt) {
	auto dtf = static_cast<float>(dt);

	_velocity += _acceleration * dtf;
	auto delta = _velocity * dtf;
	m_node->move(delta);
	auto pos =m_node->getWorldPosition();
	if (delta.y < 0 && pos.y < _yMin) {
		m_node->setPosition(pos.x, _yMin, pos.z);
		return 0;
	}
	return 1;
}

