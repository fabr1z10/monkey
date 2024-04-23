#include <iostream>
#include "text.h"
#include "../assetmanager.h"
#include "../pyhelper.h"
#include "../models/iquad.h"
#include "../util.h"
#include <glm/gtx/transform.hpp>
///#include "../vec.h"

using namespace pybind11::literals; // to bring in the `_a` literal

Text::Text(const std::string &batchId, const std::string& font, const std::string &text, const pybind11::kwargs& args) :
	Node(), _hAlign(HAlign::LEFT), _batchId(batchId), _lines(0), _offset(glm::vec3(0.f)) {
	auto* batch = dynamic_cast<QuadBatch*>(Engine::instance().getRoom()->getBatch(batchId));
	//_sheetId = batch->getSheet()->getId();
	_font = batch->getSheet()->getFont(font).get();

	_lineHeight = _font->getLineHeight();
	_width = py_get_dict<float>(args, "width", std::numeric_limits<float>::infinity());
	_pal = py_get_dict<int>(args, "pal", 0);
	//buildQuads();
	if (_width != std::numeric_limits<float>::infinity()) {
		_hAlign = static_cast<HAlign>(py_get_dict<int>(args, "halign", static_cast<int>(HAlign::LEFT)));
	}
	auto aa = py_get_dict<int>(args, "anchor", 0);
	_anchor = static_cast<Anchor>(aa);

	updateText(text);



}



void Text::updateText(const std::string & text) {

	auto s32 = getString32(text);

	float crl = 0.f;		// current row length
	float cel = 0.f;
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
			cel = 0;
			cwl = 0;
		} else {

			const auto &charInfo = _font->getCharInfo(c);

			if (c == 0x20) {
				eol = i;
				cel = crl;
				crl += charInfo.advance;
				cws = -1;
				cwl = 0;
			} else {
				if (cws == -1) cws = i;
				crl += charInfo.advance;
				cwl += charInfo.advance;
				if (crl > _width) {
					rows.back().indexEnd = eol;
					rows.back().length = cel;
					rows.emplace_back(TextRow(cws));
					crl = cwl;
					cel = 0;
				}
			}
		}
		i++;
	}
	rows.back().length = crl;


	// now that we have defined the rows we can create the quads
	auto model = std::make_shared<IQuads>(_batchId);

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
				auto kwargs = pybind11::dict("pal"_a=_pal, "pos"_a=glm::vec3(x,y,0.f),  "size"_a=glm::vec2(c.w,c.h), "normalized"_a=true);
				model->addQuad(glm::vec4(c.tx, c.ty, c.tw, c.th), kwargs);

				x += c.advance;
			}
		}
		y -= _lineHeight;
	}





	int rowCount {0};
	float maxRowLength = 0;
	for (const auto& row : rows) {
		maxRowLength = std::max(row.length, maxRowLength);
		int ec = row.indexEnd == -1 ? text.length() : row.indexEnd;
		int len = ec - row.indexStart;
		if (len > 0) {
			std::cout << text.substr(row.indexStart, len) << "\n";
			rowCount++;
		}
	}
	std::cout << "n rows = " << rowCount << "\n";
	setModel(model, pybind11::dict("batch"_a=_batchId));

	_size.x = maxRowLength;
	_size.y = rows.size() * _lineHeight;

	switch (_anchor) {
		case Anchor::CENTER:
			_offset.x = - _size.x * 0.5f;
			_offset.y = _size.y * 0.5f;
			break;
	    case Anchor::TOPRIGHT:
	        _offset.x = - _size.x;
	        break;
	    case Anchor::BOTTOMLEFT:
	        _offset.y = _size.y;
	        break;
	}

	getComponent<Renderer>()->setTransform(glm::translate(_offset));

    //Node::move(glm::vec3(_offset.x, _offset.y, _offset.z));

}

//void Text::setPosition(float x, float y, float z) {
//    Node::setPosition(x + _offset.x, y + _offset.y, z);
//}