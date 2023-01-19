#pragma once


#include "../shape.h"

class Segment3D : public Shape {
public:
	Segment3D(glm::vec3 A, glm::vec3 B);
//	bool isInside(glm::vec2 P) const override
private:
	glm::vec3 m_A;
	glm::vec3 m_B;
};
