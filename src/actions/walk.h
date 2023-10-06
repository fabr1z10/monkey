#pragma once

#include "nodeaction.h"
#include <glm/glm.hpp>
#include "../components/scummcharacter.h"


class Walk : public NodeAction {
public:
	Walk(const pybind11::kwargs&);
	void start() override;
	int process(double) override;
private:
	struct WalkSegment {
		// unit vector
		glm::vec2 direction;

		float length;

		glm::vec2 endPoint;


	};
	glm::vec2 _target;
	std::vector<WalkSegment> _segments;
	int _current;
	float _currentLength;			// distance walked in current segment
	float _speed;
	int _returnValue;
	ScummCharacter* _sc;
};