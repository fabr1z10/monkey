#include "model.h"
#include "components/renderer.h"

Model::Model() : m_vbo(GL_INVALID_VALUE), m_ebo(GL_INVALID_VALUE), m_texId(GL_INVALID_VALUE), m_paletteId(GL_INVALID_VALUE) {}

Model::Model(int shaderType, GLuint primitive) : m_shaderType(static_cast<ShaderType>(shaderType)), m_primitive(primitive),
    m_vbo(GL_INVALID_VALUE), m_ebo(GL_INVALID_VALUE), m_texId(GL_INVALID_VALUE), m_paletteId(GL_INVALID_VALUE)
{}

Model::~Model() {
    // Cleanup VBO
    glDeleteBuffers(1, &m_vbo);
    glDeleteBuffers(1, &m_ebo);
}

void Model::draw(Shader* s, int offset, int size) {



    if (size == 0) size = m_size;
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    s->setupVertices();
    glDrawElements(m_primitive, size, GL_UNSIGNED_INT, (GLvoid*)(sizeof(GLuint) * offset));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

std::shared_ptr<Renderer> Model::getRenderer() const {
    return std::make_shared<Renderer>(m_texId, m_paletteId);
}

void Model::generateBuffers(const std::vector<float>& vertices, const std::vector<unsigned>& indices) {
    // generate buffers
    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &m_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW);

    m_size = indices.size();
}
