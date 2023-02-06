#pragma once

#include "nodeaction.h"
#include <glm/glm.hpp>

class Move : public NodeAction {
public:
	explicit Move(const pybind11::kwargs&);
	int run(double) override;

private:
	int m_next;
	int m_tickCount;
	std::vector<int> m_ticks;
	std::vector<glm::vec3> m_positions;
};

