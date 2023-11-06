#pragma once

#include "../node.h"
#include <glm/glm.hpp>
#include <pybind11/pybind11.h>


class Road : public Node {


public:
	Road(const pybind11::kwargs& args);

private:
	glm::vec4 _roadColor0;
	glm::vec4 _roadColor1;
	glm::vec4 _terrainColor0;
	glm::vec4 _terrainColor1;
	glm::vec4 _stripeColor;
	int _nVertexPerRoadSeg;
	float _dz;					// z distance of road segment
	float _z;					// current player position
	int _n;						// number of segments per color switch
};

//class RoadRenderer : public BatchRenderer<