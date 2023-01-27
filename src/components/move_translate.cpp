#include "move_translate.h"
#include "../pyhelper.h"
#include "../node.h"

MoveTranslate::MoveTranslate(const pybind11::kwargs& args) {
	auto pts = args["points"];
	//m_z = dictget<float>(args, "z", 0.f);
	m_loopType = py_get_dict<int>(args, "loop", 0);
	int current{0};
	float previousTime{0.f};
	auto previous = py_get_dict<glm::vec2>(args, "start");
	for (const auto& p : pts) {
		auto point = p.cast<pybind11::dict>();
		auto pos = py_get_dict<glm::vec2>(point, "pos");
		auto time = py_get_dict<float>(point, "t");
		auto z = py_get_dict<float>(point, "z", 0.f);
		if (current > 0) {
			Step step;
			step.startPosition = previous;
			step.endPosition = pos;
			step.endTime = time;
			step.z = z;
			step.velocity = (pos - previous) / (time - previousTime);
			m_steps.push_back(step);
		}
		current++;
		previousTime = time;
		previous = pos;
	}
	m_maxIndex = m_steps.size()-1;
}


void MoveTranslate::start() {
	m_index = 0;
	const auto& step = m_steps[0];
	m_node->setPosition(step.startPosition.x, step.startPosition.y, m_z);
	m_time = 0.f;
}

void MoveTranslate::update(double dt) {
	auto dtf = static_cast<float>(dt);
	m_time += dtf;
	const auto& currentStep = m_steps[m_index];
	glm::vec3 delta;
	if (m_time < currentStep.endTime) {
		delta = glm::vec3(currentStep.velocity * dtf, 0.f);
	} else {
		delta = glm::vec3(currentStep.endPosition, 0.f) - m_node->getLocalPosition();
		m_index++;
		if (m_index > m_maxIndex) {
			m_index = 0;
			m_time = 0;
			delta = glm::vec3(m_steps[0].startPosition, 0.f) - m_node->getLocalPosition();
		}
	}
	m_node->move(delta);

}
