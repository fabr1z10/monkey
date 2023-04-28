#pragma once

#include <GL/glew.h>
#include <list>
#include "camera.h"

class Shader;

class Batch {


public:
    Batch(GLenum primitiveType, unsigned vertsPerPrim);
    virtual ~Batch();
    virtual void draw(Shader*) ;
    void cleanUp();
    int getPrimitiveId();
    void configure(Shader*);
    virtual void computeOffsets(GLuint) = 0;
    Camera* getCamera();
protected:
    // index of next primitive
    GLenum _prim;
    unsigned _verticesPerPrimitive;
    int _maxPrimitives;
    int _nPrimitive;
    GLuint _vao;
    GLuint _vbo;
    GLubyte* m_quadInfoBuffer = NULL;
    GLint _blockSize;
    std::list<int> _deallocated;
    std::shared_ptr<Camera> _cam;
    ShaderType _shaderType;
    GLuint m_uniformBuffer = 0;

};