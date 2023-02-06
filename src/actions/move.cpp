#include "move.h"
#include "../pyhelper.h"
#include "../node.h"

Move::Move(const pybind11::kwargs& args) : NodeAction(args) {
	for (const auto& p : args["frames"].cast<pybind11::dict>()) {
		auto tick = p.first.cast<int>();
		auto pos = py_get<glm::vec3>(p.second.cast<pybind11::object>());
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