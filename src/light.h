#pragma once

#include <glm/glm.hpp>
#include "shader.h"
#include <pybind11/pybind11.h>

class Light {
public:
	virtual void setup(Shader*) = 0;
};


class DirectionalLight : public Light {
public:
	DirectionalLight(const pybind11::kwargs& args);
	void setup(Shader*) override;
private:
	glm::vec3 m_direction;
	//glm::vec4 m_ambientColor;
	glm::vec3 m_diffuseColor;
};

