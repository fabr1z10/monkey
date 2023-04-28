#pragma once

#include "batch.h"

#include <string>
#include <glm/glm.hpp>
#include <pybind11/pytypes.h>
#include "symbols.h"




class SpriteBatch : public Batch {
public:
    SpriteBatch(const pybind11::kwargs&);

    //SpriteBatch(int maxQuads, const std::string& sheet);
    void draw(Shader*) override;
    std::string getSheet() const;

    //void configure(Shader*) override;
    virtual void computeOffsets(GLuint) override;

    void setQuad(int index, glm::vec3 worldPos, glm::vec2 size, glm::vec4 textureCoords,
                 glm::vec2 repeat, int paletteIndex, bool flipx, bool flipy);



private:
    struct {
        GLint Pos       = 0;
        GLint Size      = 0;
        GLint TexCoords = 0;
        GLint Repeat    = 0;
        GLint Palette   = 0;
    } m_quadInfoOffsets;
    struct V2 {
        glm::vec2 pos;
        GLuint quadId;

    };

    std::string _sheet;
    GLuint _texId;
    GLuint _paletteId;
    int _paletteCount;
    float _invPaletteCount;
};

inline std::string SpriteBatch::getSheet() const {
    return _sheet;
}


