#include <iostream>
#include "text.h"
#include "../assetmanager.h"
#include "../pyhelper.h"
#include "../models/iquad.h"
#include "../util.h"

#include "../vec.h"

using namespace pybind11::literals; // to bring in the `_a` literal

Text::Text(const std::string &batchId, const std::string& font, const std::string &text, const pybind11::kwargs& args) :
	Node(), _hAlign(HAlign::LEFT), _batchId(batchId) {
	auto* batch = dynamic_cast<QuadBatch*>(Engine::instance().getRoom()->getBatch(batchId));
	_sheetId = batch->getSheet()->getId();
	_font = batch->getSheet()->getFont(font).get();

	_lineHeight = _font->getLineHeight();
	_width = py_get_dict<float>(args, "width", std::numeric_limits<float>::infinity());
	//_pal = py_get_dict<int>(args, "pal", 0);
	//buildQuads();
	if (_width != std::numeric_limits<float>::infinity()) {
		_hAlign = static_cast<HAlign>(py_get_dict<int>(args, "halign", static_cast<int>(HAlign::LEFT)));
	}

	updateText(text);



}

void Text::updateText(const std::string & text) {

	auto s32 = getString32(text);

	float crl = 0.f;		// current row length
	float cwl = 0.f;		// current world length
	int eol = -1;		// eol index
	int cws = -1;

	//std::vector<std::pair<int, int>> rows;
	std::vector<TextRow> rows;
	rows.emplace_back(TextRow(0));
	int i = 0;
	for (const auto& c : s32) {

		if ( c == '\n') {
			rows.back().indexEnd = i;
			rows.back().length = crl;
			rows.emplace_back(TextRow(i+1));
			eol = 0;
			crl = 0;
			cwl = 0;
		} else {

			const auto &charInfo = _font->getCharInfo(c);

			if (c == 0x20) {
				eol = i;
				crl += charInfo.advance;
				cws = -1;
				cwl = 0;
			} else {
				if (cws == -1) cws = i;
				crl += charInfo.advance;
				cwl += charInfo.advance;
				if (crl > _width) {
					rows.back().indexEnd = eol;
					rows.emplace_back(TextRow(cws));
					crl = cwl;
				}
			}
		}
		i++;
	}



	// now that we have defined the rows we can create the quads
	auto model = std::make_shared<IQuads>(_sheetId);

	float y = -_lineHeight;

	for (const auto& row : rows) {
		float x{0.f};
		if (_hAlign == HAlign::CENTER) {
			x = (_width - row.length) * 0.5f;
		} else if (_hAlign == HAlign::RIGHT) {
			x = (_width - row.length);
		}
		int ec = row.indexEnd == -1 ? text.length() : row.indexEnd;
		int len = ec - row.indexStart;
		if (len > 0) {
			for (int i = row.indexStart; i < ec; ++i) {
				const auto &c = _font->getCharInfo(s32[i]);
				glm::vec3 p(x,y,0.f);
				auto kwargs = pybind11::dict("pal"_a=_pal, "pos"_a=vec3(x,y,0.f),  "size"_a=glm::vec2(c.w,c.h), "normalized"_a=true);
				model->addQuad(glm::vec4(c.tx, c.ty, c.tw, c.th), kwargs);

				x += c.advance;
			}
		}
		y -= _lineHeight;
	}





	int rowCount {0};
	for (const auto& row : rows) {
		int ec = row.indexEnd == -1 ? text.length() : row.indexEnd;
		int len = ec - row.indexStart;
		if (len > 0) {
			std::cout << text.substr(row.indexStart, len) << "\n";
			rowCount++;
		}
	}
	std::cout << "n rows = " << rowCount << "\n";
	setModel(model, pybind11::dict("batch"_a=_batchId));
//
//	auto size = glm::vec2(0.f);
//
//	int quadCount = 0;
//	std::vector<float> rowLength;
//	int lineCount = 1;
//
//	// rowstart[i] holds the index of the 1st character in row i
//	std::vector<int> rowStart {0};
//
//	glm::vec3 position(0.f, -_lineHeight, 0.f);
//
//	size_t indexBeginCurrentWord = 0;
//
//	auto& quads = model->getQuads();
//	for (const auto& c : s32) {
//		//font->get
//		// one quad per character
//		if (c == '\n') {
//			rowLength.push_back(position.x);
//			position.x = 0;
//			position.y -= _lineHeight;
//			rowStart.push_back(quadCount);
//			lineCount++;
//			indexBeginCurrentWord = model->getQuadCount();
//			continue;
//		}
//
//		const auto& charInfo = _font->getCharInfo(c);
//
//		if (c == 0x20) {
//			position.x += charInfo.advance;
//			indexBeginCurrentWord = model->getQuadCount();
//			continue;
//		}
//
//		if (position.x > _width) {
//			// reset location of current word
//			// get x of first letter
//			float x = position.x;
//			position.x = 0;
//			position.y -= _lineHeight;
//			if (indexBeginCurrentWord < model->getQuadCount()) {
//				// we need to push current word on next row
//				const auto& desc = quads[indexBeginCurrentWord-1];
//				rowLength.push_back(desc.location.x + desc.size.x);
//				glm::vec3 pos = quads[indexBeginCurrentWord].location;
//				rowStart.push_back(indexBeginCurrentWord);
//				if (pos.x > 0) {
//					x = pos.x;
//					for (size_t i = indexBeginCurrentWord; i < quads.size(); ++i) {
//						quads[i].location.x -= pos.x;
//						quads[i].location.y = position.y;
//						position.x += quads[i].advance;
//					}
//				}
//			} else {
//				rowStart.push_back(quadCount);
//				rowLength.push_back(quads.back().location.x + quads.back().size.x);
//			}
//		}
//
//		QuadInfo q;
//		q.textureCoordinates = glm::vec4(charInfo.tx, charInfo.tx + charInfo.tw, charInfo.ty, charInfo.ty + charInfo.th);
//		q.size = glm::vec2(charInfo.w, charInfo.h);
//		q.location = position;
//		//q.anchorPoint = glm::vec3(0.f, -charInfo.oy, 0.f);
//		q.palette = _pal;
//		position.x += charInfo.advance;
//		q.advance = charInfo.advance;
//		q.repeat = glm::vec2(1, 1);
//		quads.push_back(q);
//		quadCount++;
//	}
//	rowLength.push_back(position.x);
//
//	for (const auto& rl : rowLength) _size.x = std::max(_size.x, rl);
//	//_size.x = std::max(_size.x, position.x);
//	_size.y = _lineHeight * rowLength.size();
//	_lines = lineCount;
//	rowStart.push_back(quadCount);
//	_bottomLeft.x = 0.f;
//	_bottomLeft.y = -_size.y;
//	if (_hAlign != HAlign::LEFT) {
//		for (int i = 0; i < rowStart.size() - 1; ++i) {
//			std::cout << " Row " << i << " starts @ " << rowStart[i] << " and has length " << rowLength[i] << "\n";
//			// adjust position
//			for (size_t j = rowStart[i]; j < rowStart[i + 1]; j++) {
//				frame.quads[j].anchorPoint.x += rowLength[i] * (_hAlign == HAlign::CENTER ? 0.5f : 1.f);
//			}
//			_bottomLeft.x = -_size.x * (_hAlign == HAlign::CENTER ? 0.5f : 1.f);
//		}
//	}
//	if (_vAlign != VAlign::TOP) {
//		for (auto& quad : frame.quads) {
//			quad.anchorPoint.y -= (lineCount * _lineHeight) * (_vAlign == VAlign::CENTER ? 0.5f : 1.0f);
//		}
//		_bottomLeft.y += _size.y * (_vAlign == VAlign::CENTER ? 0.5f : 1.f);
//	}
//	m_modelBounds.min = glm::vec3(_bottomLeft, 0.f);
//	m_modelBounds.max = glm::vec3(_bottomLeft + _size, 0.f);
}