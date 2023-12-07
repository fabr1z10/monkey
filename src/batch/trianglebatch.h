#pragma once

#include "../batch.h"

#include <string>
#include <glm/glm.hpp>
#include <pybind11/pytypes.h>

//#include "symbols.h"
struct TriBatchVertexData {
	glm::vec3 position;
	glm::vec4 color;
} ;


class TriangleBatch : public Batch<TriBatchVertexData> {
public:
	explicit TriangleBatch(const pybind11::kwargs&);

	void initDraw(Shader* s) override {}

	void innerConfigure() override;

	void setInvisible(int index) override {}

	void setTriangle(int index, glm::vec3 P0, glm::vec3 P1, glm::vec3 P2, glm::vec4 color0, glm::vec4 color1, glm::vec4 color2);
	void setTriangle(int index, glm::vec3 P0, glm::vec3 P1, glm::vec3 P2, glm::vec4 color);
};



