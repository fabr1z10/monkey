#pragma once


#include "../batch.h"

struct LineBatchVertexData {
	glm::vec3 position;
	glm::vec4 color;
} ;


class LineBatch : public Batch<LineBatchVertexData> {
public:
    LineBatch(const pybind11::kwargs& args);
    //virtual void computeOffsets(GLuint) ;
    void setLine(int index, glm::vec3 P0, glm::vec3 P1, glm::vec4 color) ;
    void hideLines(int index, int number);
    //void draw(Shader*) override;
	void innerConfigure() override;
	void initDraw(Shader*) override {}
	void setInvisible(int index) override {}
private:


};