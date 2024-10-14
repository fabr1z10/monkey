#pragma once

#include "../component.h"
#include "../camera.h"
#include <pybind11/pybind11.h>



class Parallax : public Component {
public:
	Parallax(int camId, float z, glm::vec2 basePosition, glm::vec2 parallaxFactor, const pybind11::kwargs& args);
	void update(double) override;
	void start() override;
protected:
	glm::vec2 _basePosition;
	glm::vec2 _camBasePos;
	glm::vec2 _parallaxFactor;
	OrthoCamera* _cam;
	int _camId;
	float _z;

};