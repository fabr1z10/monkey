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
	TriangleBatch(const pybind11::kwargs&);

	void initDraw(Shader* s) override;

	void innerConfigure() override;

	void setInvisible(int index) override;

};



