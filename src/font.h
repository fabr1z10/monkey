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
    float getLineWidth() const;
private:
    GLuint _texId;
    float _lineWidth;
    std::unordered_map<char32_t, CharInfo> m_info;
};

inline float Font::getLineWidth() const {
	return _lineWidth;
}