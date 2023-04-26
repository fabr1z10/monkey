#pragma once

#include <GL/glew.h>
#include <string>
#include <vector>

class Palette {
public:
    Palette(const std::vector<unsigned char>&);
    ~Palette () = default;
    GLuint getTexId() const;
    int getPaletteCount() const;
private:
    GLuint m_texId;
    int _palCount;
};

inline GLuint Palette::getTexId() const { return m_texId; }

inline int Palette::getPaletteCount() const {
    return _palCount;
}

