#include "batch.h"
#include <iostream>
#include <cstring>


Batch::Batch(GLenum primitiveType, unsigned vertsPerPrim) : _prim(primitiveType), _verticesPerPrimitive(vertsPerPrim) {}

Batch::~Batch() {
    free(m_quadInfoBuffer);
}

int Batch::getPrimitiveId() {
    int next;
    if (_deallocated.empty()) {
        next = _nPrimitive;
        _nPrimitive++;
    } else {
        next = _deallocated.front();
        _deallocated.pop_front();
    }
    return next;
}

void Batch::draw(Shader* s) {
    if (s->getShaderType() != _shaderType) {
        return;
    }
    _cam->init(s);
    // set view matrix
    s->setMat4("view", _cam->getViewMatrix());
    glBindBuffer(GL_UNIFORM_BUFFER, m_uniformBuffer);
    glBufferData(GL_UNIFORM_BUFFER, _blockSize, m_quadInfoBuffer, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_uniformBuffer);

    glBindVertexArray(_vao);
    glDrawArrays(_prim, 0, _nPrimitive * _verticesPerPrimitive);
    glBindVertexArray(0);
}

void Batch::cleanUp() {

    _nPrimitive = 0;
    _deallocated.clear();
    memset(m_quadInfoBuffer, 0, _blockSize);
}


Camera * Batch::getCamera() {
    return _cam.get();
}


void Batch::configure(Shader * s) {
    if (s->getShaderType() != _shaderType) {
        return;
    }
    auto shaderProg = s->getProgId();
    GLuint BlockIndex = glGetUniformBlockIndex(shaderProg, "QuadInfo");

    glUniformBlockBinding(shaderProg, 0, BlockIndex);
    printf("BlockIndex %d\n", BlockIndex);

    glGetActiveUniformBlockiv(shaderProg, BlockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &_blockSize);
    printf("Size %d\n", _blockSize);

    m_quadInfoBuffer = (GLubyte*)malloc(_blockSize);

    computeOffsets(shaderProg);

    glGenBuffers(1, &m_uniformBuffer);
    printf("Uniform buffer %d\n", m_uniformBuffer);
}