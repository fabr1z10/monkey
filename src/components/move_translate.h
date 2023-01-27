#pragma once

#include "../component.h"
#include <glm/glm.hpp>
#include <pybind11/pybind11.h>

class MoveTranslate : public Component {
public:
	explicit MoveTranslate(const pybind11::kwargs&);
	void update(double) override;
	void start() override;

private:
	struct Step {
		glm::vec2 velocity;
		float endTime;
		glm::vec2 startPosition;
		glm::vec2 endPosition;
		float z;
	};
	std::vector<Step> m_steps;
	int m_loopType;     // 0 = moves back and forth, 1 = loop, 2 = end
	float m_time;
	int m_index;
	float m_z;

	int m_maxIndex;
};
