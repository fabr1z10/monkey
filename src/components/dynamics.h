#pragma once

#include "../component.h"
#include <glm/glm.hpp>
#include "../pyhelper.h"

class Dynamics : public Component {
public:
	Dynamics(const pybind11::kwargs& args) : m_acceleration(0.f) {
		m_velocity = py_get_dict<glm::vec3>(args, "velocity", glm::vec3(0.f));
	}
	void start() override {}
	void update(double dt) {
		//m_velocity += m_acceleration * static_cast<float>(dt);
	}
	void setVelocity(float vx, float vy, float vz);
	glm::vec3 m_velocity;
	glm::vec3 m_acceleration;
	using Base = Dynamics;
};

inline void Dynamics::setVelocity(float vx, float vy, float vz) {
	m_velocity = glm::vec3(vx, vy, vz);
}