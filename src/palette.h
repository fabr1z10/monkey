#pragma once

#include <GL/glew.h>
#include <string>
#include <vector>

class Palette {
public:
    Palette(const std::vector<unsigned char>&);
    ~Palette () = default;
    GLuint getTexId() const;
private:
    GLuint m_texId;

};

inline GLuint Palette::getTexId() const { return m_texId; }

