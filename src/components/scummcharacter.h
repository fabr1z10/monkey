#pragma once

#include "../component.h"
#include <glm/glm.hpp>
#include <pybind11/pybind11.h>

class ScummCharacter : public Component {
private:
	enum CharType {
		TWO_WAY = 0,
		FOUR_WAY = 1,
		EIGHT_WAY = 2
	};
public:
	ScummCharacter(const pybind11::kwargs& args);
	void setAnimation(const std::string&);
	float getSpeed() const;
	int getTextPalette() const;
	void setDirection(glm::vec2);
	using Base = ScummCharacter;
private:
	CharType _charType;
	float _speed;
	// the direction actor is facing
	glm::vec2 _direction;
	// text pal
	int _textPalette;
};

inline float ScummCharacter::getSpeed() const {
	return _speed;
}

inline void ScummCharacter::setDirection(glm::vec2 dir) {
	_direction = dir;
}

inline int ScummCharacter::getTextPalette() const {
	return _textPalette;
}