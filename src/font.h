#pragma once


#include <GL/glew.h>
#include <unordered_map>

struct CharInfo {
    // texture top left
    float tx, ty;
    // size
    float tw, th;
    // width and height in px
    float w, h;
    float advance;
    float oy;
};

class Font {
public:
    Font(const std::string& file);
    const CharInfo& getCharInfo(char32_t c);
private:
    GLuint _texId;
    std::unordered_map<char32_t, CharInfo> m_info;
};