#pragma once

// this special class is just a box of fixed size that cannot rotate.
// this is ok for handling collisions in platformer or other games where
// the nodes can only translate but never rotate!

#include "../shape.h"
#include <array>

class AABB3D : public Shape {
public:
	AABB3D(float xmin, float xmax, float ymin, float ymax, float zmin, float zmax) :
		_xMin(xmin), _xMax(xmax), _yMin(ymin), _yMax(ymax), _zMin(zmin), _zMax(zmax) {}
	bool isInside(glm::vec3 P) const override {
		return P.x >= _xMin && P.x <= _xMax && P.y >= _yMin && P.y <= _yMax && P.z >= _zMin && P.z <= _zMax;
	}
private:
	float _xMin;
	float _xMax;
	float _yMin;
	float _yMax;
	float _zMin;
	float _zMax;
};
