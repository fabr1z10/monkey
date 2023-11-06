#include "trianglebatch.h"


void TriangleBatch::innerConfigure() {

	auto stride = sizeof(TriBatchVertexData);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (const void *) offsetof(TriBatchVertexData, color));

}