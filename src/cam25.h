#pragma once

#include "camera.h"

class Camera25 : public OrthoCamera {
public:
	Camera25(float width, float height, const pybind11::kwargs& args);
	void setPosition(glm::vec3 eye, glm::vec3 direction, glm::vec3 up) override;

private:
	float m_angle;
	float m_cosAngle;
	float m_sinAngle;
};