#include "text.h"
#include "../pyhelper.h"
#include "../util.h"
#include "../font.h"
#include "../assetmanager.h"
#include "../engine.h"
#include <iostream>

std::shared_ptr<Renderer> Text::getRenderer(const pybind11::kwargs & args) {
	return std::make_shared<SpriteRenderer>(args);
}

void Text::buildQuads() {
	auto s32 = getString32(_text);


	//_batch = font->getBatch();

	Animation anim;
	Frame frame;
	_quadCount = 0;
	std::vector<float> rowLength;
	int lineCount = 1;
	std::vector<int> rowStart {0};

	glm::vec3 position(0.f, -_font->getLineHeight(), 0.f);
	for (const auto& c : s32) {
		//font->get
		// one quad per character
		Desc quad;

		if (c == '\n') {
			std::cout << " suca newline\n";
			rowLength.push_back(position.x);
			_size.x = std::max(_size.x, position.x);
			position.x = 0;
			position.y -= _font->getLineHeight();
			rowStart.push_back(_quadCount);
			lineCount++;
			continue;
		}


		const auto& charInfo = _font->getCharInfo(c);
		//std::cout << "ciao " << charInfo.advance << "\n";
		if (c == 0x20) {
			position.x += charInfo.advance;
			continue;


		}

		quad.textureCoordinates = glm::vec4(charInfo.tx, charInfo.tx + charInfo.tw, charInfo.ty, charInfo.ty + charInfo.th);
		quad.size = glm::vec2(charInfo.w, charInfo.h);
		quad.location = position;
		quad.anchorPoint = glm::vec3(0.f, -charInfo.oy, 0.f);
		position.x += charInfo.advance;
		quad.repeat = glm::vec2(1, 1);
		frame.quads.push_back(quad);
		_quadCount++;
	}
	rowLength.push_back(position.x);
	_size.x = std::max(_size.x, position.x);
	_size.y = _font->getLineHeight() * rowLength.size();

	rowStart.push_back(_quadCount);
	_bottomLeft.x = 0.f;
	_bottomLeft.y = -_size.y;
	if (_hAlign != HAlign::LEFT) {
		for (int i = 0; i < rowStart.size() - 1; ++i) {
			std::cout << " Row " << i << " starts @ " << rowStart[i] << " and has length " << rowLength[i] << "\n";
			// adjust position
			for (size_t j = rowStart[i]; j < rowStart[i + 1]; j++) {
				frame.quads[j].anchorPoint.x += rowLength[i] * (_hAlign == HAlign::CENTER ? 0.5f : 1.f);
			}
			_bottomLeft.x = -_size.x * (_hAlign == HAlign::CENTER ? 0.5f : 1.f);
		}
	}
	if (_vAlign != VAlign::TOP) {
		for (auto& quad : frame.quads) {
			quad.anchorPoint.y -= (lineCount * _font->getLineHeight()) * (_vAlign == VAlign::CENTER ? 0.5f : 1.0f);
		}
		_bottomLeft.y += _size.y * (_vAlign == VAlign::CENTER ? 0.5f : 1.f);
	}
	m_modelBounds.min = glm::vec3(_bottomLeft, 0.f);
	m_modelBounds.max = glm::vec3(_bottomLeft + _size, 0.f);
	anim.frames.push_back(frame);
	_defaultAnimation = "main";
	_animations["main"] = anim;
}

Text::Text(const pybind11::kwargs & args) : IQuad() {

	_hAlign = static_cast<HAlign>(py_get_dict<int>(args, "halign", static_cast<int>(HAlign::LEFT)));

	_vAlign = static_cast<VAlign>(py_get_dict<int>(args, "valign", static_cast<int>(VAlign::TOP)));

	auto fontId = py_get_dict<std::string>(args, "font");
	auto font = AssetManager::instance().getFont(fontId);
	_font = font.get();

    _text = py_get_dict<std::string>(args, "text");
	buildQuads();

}

void Text::setText(const std::string &value) {
	_text = value;
	_animations.clear();
	buildQuads();
}

