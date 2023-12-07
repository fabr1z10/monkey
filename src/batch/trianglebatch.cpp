#include "trianglebatch.h"

TriangleBatch::TriangleBatch(const pybind11::kwargs& args) : Batch(3, 3, args) {
	_prim = GL_TRIANGLES;
	_shaderType = ShaderType::BATCH_LINES;

}

void TriangleBatch::innerConfigure() {

	auto stride = sizeof(TriBatchVertexData);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (const void *) offsetof(TriBatchVertexData, color));

	// this depends on the particular batch and should go in a virtual method
	std::vector<unsigned> indices;
	for (size_t i = 0; i < _maxElements; ++i) {
		indices.push_back(3 * i);
		indices.push_back(3 * i + 1);
		indices.push_back(3 * i + 2);
	}

	glGenBuffers(1, &_ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW);

}
void TriangleBatch::setTriangle(int index, glm::vec3 P0, glm::vec3 P1, glm::vec3 P2, glm::vec4 color0, glm::vec4 color1,
								glm::vec4 color2) {
	_data[index].position = P0;
	_data[index].color = color0;
	_data[index+1].position = P1;
	_data[index+1].color = color1;
	_data[index+2].position = P2;
	_data[index+2].color = color2;

}



void TriangleBatch::setTriangle(int index, glm::vec3 P0, glm::vec3 P1, glm::vec3 P2, glm::vec4 color) {
	int offset = index * _vertsPerElement;

	_data[offset].position = P0;
	_data[offset].color = color;
	_data[offset+1].position = P1;
	_data[offset+1].color = color;
	_data[offset+2].position = P2;
	_data[offset+2].color = color;

}