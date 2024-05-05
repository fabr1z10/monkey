#pragma once

#include "../node.h"
#include "../font.h"


// align each row left (default), center, or right w.r.t. box
// this only works if a width is specified

enum class HAlign {
	LEFT = 0,
	CENTER = 1,
	RIGHT = 2
};


enum class Anchor {
	TOPLEFT = 0,
	TOPRIGHT = 1,
	BOTTOMLEFT = 2,
	BOTTOMRIGHT = 3,
	CENTER = 4
};

class Text : public Node {

public:
	Text(const std::string& batchId, const std::string& font, const std::string& text, const pybind11::kwargs& args);
	void updateText(const std::string&);
	std::string getText() const;
	glm::vec2 getSize() const;
private:
	struct TextRow {
		TextRow (int iStart) : indexStart(iStart), indexEnd(-1), length(0.f) {}
		int indexStart;
		int indexEnd;
		float length;
	};
	float _width;
	float _lineHeight;
	Font* _font;
	int _pal;
	glm::vec2 _size;
	glm::vec3 _offset;
	int _lines;
	HAlign _hAlign;
	Anchor _anchor;
	//std::string _sheetId;
	std::string _batchId;
	std::string _text;
    //void setPosition(float, float, float) override;


};

inline std::string Text::getText() const {
	return _text;
}

inline glm::vec2 Text::getSize() const {
	return _size;
}