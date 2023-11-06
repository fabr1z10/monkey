#pragma once

#include "../model.h"
#include "../symbols.h"

class DrawableModel : public Model {
public:
	DrawableModel(GLuint primitive) : _primitive(primitive), _texId(GL_INVALID_VALUE), _size(0) {}

	void initModel(const std::vector<float>& vertices, const std::vector<unsigned>& indices);
	void draw(Shader*);

protected:
	ShaderType m_shaderType;

	GLuint _size;
	GLuint _primitive;
	GLuint _texId;
	GLuint _vao;
	GLuint _vbo;
	GLuint _ebo;
};