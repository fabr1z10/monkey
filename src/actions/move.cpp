#include "move.h"
#include "../pyhelper.h"
#include "../node.h"
#include <glm/gtx/transform.hpp>

Move::Move(const pybind11::kwargs& args) : NodeAction(args) {
	for (const auto& p : args["frames"].cast<pybind11::dict>()) {
		auto tick = p.first.cast<int>();
		auto pos = p.second.cast<glm::vec3>();//py_get<glm::vec3>(p.second.cast<pybind11::object>());
		m_ticks.push_back(tick);
		m_positions.push_back(pos);

	}
	m_next = 0;
	m_tickCount = 0;
}

int Move::run(double) {
	if (m_tickCount == m_ticks[m_next]) {
		const auto& pos = m_positions[m_next];
		m_node->setPosition(pos.x, pos.y, pos.z);
		m_next ++;
		if (m_next >= m_ticks.size()) return 0;
	}
	m_tickCount ++;
	return 1;
}

MoveBy::MoveBy(const pybind11::kwargs& args) : NodeAction(args) {
	auto dx = py_get_dict<float>(args, "x", 0.f);
	auto dy = py_get_dict<float>(args, "y", 0.f);
	auto dz = py_get_dict<float>(args, "z", 0.f);
	auto t = py_get_dict<float>(args, "t", -1.f);
	m_distance = sqrt(dx * dx + dy * dy);
	m_delta = glm::vec3(dx, dy, 0.f);
	m_unitVec = glm::normalize(m_delta);
	if (t < 0) {
		m_speed = args["speed"].cast<float>();
	} else {
		m_speed = m_distance / t;
	}

}

void MoveBy::start() {
	NodeAction::start();

	m_endPoint = m_node->getWorldPosition() + m_delta;
	m_distanceTraveled = 0.f;
}


int MoveBy::run(double dt) {
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