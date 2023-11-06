#include "drawablemodel.h"
#include "../engine.h"

void DrawableModel::initModel(const std::vector<float> &vertices, const std::vector<unsigned int> &indices) {
	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	// generate buffers
	glGenBuffers(1, &_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &_ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW);

	_size = indices.size();

	GLuint i{0};
	unsigned long ptr{0};

	auto * shader = Engine::instance().getShader(m_shaderType);
	shader->setupVertices();
	glBindVertexArray(0);
}

void DrawableModel::draw(Shader* s) {


	if (_texId != GL_INVALID_VALUE) {
		s->setInt("texture_diffuse1", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, _texId);
	}

	glBindVertexArray(_vao);
	glDrawElements(_primitive, _size, GL_UNSIGNED_INT, (GLvoid*)(sizeof(GLuint) * 0));
	glBindVertexArray(0);
}