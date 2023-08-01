#pragma once


#include <GL/glew.h>
#include <unordered_map>
#include "batch/quadbatch.h"
#include <yaml-cpp/yaml.h>

class SpriteSheet;


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
    Font(SpriteSheet* sheet, const YAML::Node& node);
    const CharInfo& getCharInfo(char32_t c);
    float getLineHeight() const;
    QuadBatch* getBatch();
private:
    QuadBatch* _batch;
    float _lineHeight;
    std::unordered_map<char32_t, CharInfo> m_info;
    SpriteSheet* _sheet;
};

inline float Font::getLineHeight() const {
	return _lineHeight;
}

