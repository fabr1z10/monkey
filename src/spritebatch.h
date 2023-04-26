#pragma once

#include <GL/glew.h>
#include <list>
#include <string>
#include <glm/glm.hpp>
#include <pybind11/pytypes.h>
#include "symbols.h"
#include "camera.h"


class Shader;

class SpriteBatch {

public:
    SpriteBatch(const pybind11::kwargs&);
    ~SpriteBatch();
    //SpriteBatch(int maxQuads, const std::string& sheet);
    void draw(Shader*);
    std::string getSheet() const;
    int getQuadId();
    void configure(Shader*);
    void setQuad(int index, glm::vec3 worldPos, glm::vec2 size, glm::vec4 textureCoords,
                 glm::vec2 repeat);
    void cleanUp();

    Camera* getCamera();

private:
    struct {
        GLint Pos       = 0;
        GLint Size      = 0;
        GLint TexCoords = 0;
        GLint Repeat    = 0;
    } m_quadInfoOffsets;
    struct V2 {
        glm::vec2 pos;
        GLuint quadId;

    };

    ShaderType _shaderType;
    int _maxQuads;
    int _nQuads;
    GLuint _vao;
    GLuint _vbo;
    std::string _sheet;
    std::list<int> _deallocated;
    GLuint _texId;
    GLuint _paletteId;
    GLint _blockSize;
    GLuint m_uniformBuffer = 0;
    GLubyte* m_quadInfoBuffer = NULL;
    std::shared_ptr<Camera> _cam;
};

inline std::string SpriteBatch::getSheet() const {
    return _sheet;
}


inline Camera * SpriteBatch::getCamera() {
    return _cam.get();
}
