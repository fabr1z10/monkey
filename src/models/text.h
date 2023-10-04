#pragma once


#include "iquad.h"
#include <pybind11/pybind11.h>

class Font;

enum class HAlign {
	LEFT = 0,
	CENTER = 1,
	RIGHT = 2
};

enum class VAlign {
	TOP = 0,
	CENTER = 1,
	BOTTOM = 2
};

class Text : public IQuad {
public:
	Text(const pybind11::kwargs&);
    // text requires: a font, a text, a palette
    // then we might have some subclass ... like text appearing one character at a time
    //void initFromPy(const pybind11::kwargs&) override;
    //void initFromYAML(const YAML::Node &node, const YAML::Node &globals) override { throw; }
    void setText(const std::string& value);
    std::string getText() const;
	std::shared_ptr<Renderer> getRenderer(const pybind11::kwargs&) override;
	glm::vec2 getSize() const;
	glm::vec2 getBottomLeft() const;
	int getLines() const;
private:
	void buildQuads();
	Font* _font;
	glm::vec2 _size;
	glm::vec2 _bottomLeft;
	HAlign _hAlign;
	VAlign _vAlign;
	std::string _text;
	int _lines;
	float _width;
	float _lineHeight;

};

inline int Text::getLines() const {
	return _lines;
}

inline glm::vec2 Text::getSize() const {
	return _size;
}

inline glm::vec2 Text::getBottomLeft() const {
	return _bottomLeft;
}

inline std::string Text::getText() const {
	return _text;
}