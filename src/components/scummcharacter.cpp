#include "scummcharacter.h"
#include "../pyhelper.h"
#include "../node.h"

ScummCharacter::ScummCharacter(const pybind11::kwargs &args) {
	_speed = py_get_dict<float>(args, "speed");
	_charType = static_cast<CharType>(py_get_dict<int>(args, "type", 1));
	_textPalette = py_get_dict<int>(args, "text_pal", 0);
}

void ScummCharacter::setAnimation(const std::string & anim) {
	std::string ds;
	bool flipHorizontal {false};
	if (_charType == CharType::FOUR_WAY) {
		if (fabs(_direction.x) >= fabs(_direction.y)) {
			ds = "_e";
			flipHorizontal = _direction.x < 0;
		} else {
			ds = (_direction.y > 0 ? "_n" : "_s");
		}
	} else if (_charType == CharType::EIGHT_WAY) {

	}
	// for two way no attachment

	std::string a = anim + ds;
	m_node->setAnimation(a);
	m_node->setFlipX(flipHorizontal);


}
