#include "move_translate.h"
#include "../pyhelper.h"
#include "../node.h"

MoveTranslate::MoveTranslate(const pybind11::kwargs& args) : m_time(0.f), m_index(0) {
	auto pts = py_get_dict<pybind11::list>(args, "points");

	//m_z = dictget<float>(args, "z", 0.f);
	m_loopType = py_get_dict<int>(args, "loop", 0);

	float previousTime{0.f};
	auto previous = py_get_dict<glm::vec2>(pts[0], "pos");// py_get_dict<glm::vec2>(args, "start");

	for (size_t i = 1; i < pts.size(); ++i) {
		auto pos = py_get_dict<glm::vec2>(pts[i], "pos");
		auto time = py_get_dict<float>(pts[i], "t");
		auto z = py_get_dict<float>(pts[i], "z", 0.f);
		Step step;
		step.startPosition = previous;
		step.endPosition = pos;
		step.endTime = time;
		step.length = time - previousTime;
		step.z = z;
		step.velocity = (pos - previous) / (time - previousTime);
		m_steps.push_back(step);
		previousTime = time;
		previous = pos;
	}
	if (m_loopType == 1) {
		float currentTime = m_steps.back().endTime;
		for (int i = m_steps.size()-1; i >= 0; --i) {
			Step s;
			s.startPosition = m_steps[i].endPosition;
			s.endPosition = m_steps[i].startPosition;
			s.z = m_steps[i].z;
			s.endTime = currentTime + m_steps[i].length;
			s.length = m_steps[i].length;
			s.velocity = -m_steps[i].velocity;
			m_steps.push_back(s);
		}
	}
	m_maxIndex = m_steps.size()-1;
}


void MoveTranslate::start() {
	m_index = 0;
	const auto& step = m_steps[0];
	m_node->setPosition(step.startPosition.x, step.startPosition.y, step.z);
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
		float currentZ = m_steps[m_index].z;
		m_index++;
		if (m_index > m_maxIndex) {
			m_index = 0;
			m_time = 0;
			delta = glm::vec3(m_steps[0].startPosition, 0.f) - m_node->getLocalPosition();
		}
		delta.z = m_steps[m_index].z -currentZ;
	}
	m_node->move(delta);

}
