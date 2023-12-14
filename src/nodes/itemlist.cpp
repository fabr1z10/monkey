#include <iostream>
#include "itemlist.h"
#include "../models/text.h"
#include "../assetmanager.h"
#include <GLFW/glfw3.h>
#include "../error.h"
#include "../pyhelper.h"
#include "../components/texthotspot.h"
#include "../shapes/aabb.h"

using namespace pybind11::literals; // to bring in the `_a` literal

ArrowHotSpot::ArrowHotSpot(std::shared_ptr<Shape> shape, int palUnselected, int palSelected, int dir, const pybind11::kwargs &args) :
	HotSpot(shape, args), _palUnselected(palUnselected), _palSelected(palSelected), _dir(dir) {}

void ArrowHotSpot::start() {
	HotSpot::start();
	_itemList = dynamic_cast<ItemList*>(m_node->getParent());
	M_Assert(_itemList != nullptr, "ciao");
}

void ArrowHotSpot::onEnter() {
	m_node->setPalette(_palSelected);
}

void ArrowHotSpot::onLeave() {
	m_node->setPalette(_palUnselected);
}

void ArrowHotSpot::onClick(glm::vec2 pos, int button, int action) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		if (_dir == -1) {
			_itemList->down();
		} else {
			_itemList->up();
		}
	}
}

ItemList::ItemList(const pybind11::kwargs& inargs) : Node(), _itemCount(0), _rowSelected(0), _rowTop(0), _nextRow(0), _arrowDown(nullptr), _arrowUp(nullptr) {
	_font = py_get_dict<std::string>(inargs, "font");
	_height = py_get_dict<int>(inargs, "height");
	_callback = py_get_dict<pybind11::function>(inargs, "callback", pybind11::function());
	_batch = py_get_dict<std::string>(inargs, "batch");
	_useMouse = py_get_dict<bool>(inargs, "use_mouse", false);
	_palId = py_get_dict<int>(inargs, "palette", 0);
	if (_useMouse) {
		_onEnter = py_get_dict<pybind11::function>(inargs, "on_enter", pybind11::function());
		_onLeave = py_get_dict<pybind11::function>(inargs, "on_leave", pybind11::function());
		_onClick = py_get_dict<pybind11::function>(inargs, "on_click", pybind11::function());
	}


	auto f = AssetManager::instance().getFont(_font);
	_lineHeight = py_get_dict<float>(inargs, "line_height", f->getLineHeight());

//	auto cursor = std::make_shared<Node>();
//	pybind11::kwargs args;
//	args["text"] = "A";
//	args["font"] = _font;
//	cursor->setModel(std::make_shared<Text>(args), pybind11::dict("batch"_a = _batch));
//	add(cursor);
//	cursor->setPosition(0.f, 0.f, 0.f);
//	_cursor = cursor.get();
	_offsetx=8;
	_listener = std::make_unique<ItemListKeyListener>(this);
	_arrowPalUnselected = py_get_dict<int>(inargs, "arrow_palette", 0);
	_arrowPalSelected = py_get_dict<int>(inargs, "arrow_palette_selected", 0);
	auto arrowDown = py_get_dict<std::string>(inargs, "arrow_down", "");
	auto arrowDownPos = py_get_dict<glm::vec3>(inargs, "arrow_down_pos", glm::vec3(0.f));
	auto arrowUpPos = py_get_dict<glm::vec3>(inargs, "arrow_up_pos", glm::vec3(0.f));
	auto arrowUp = py_get_dict<std::string>(inargs, "arrow_up", "");
	if (!arrowDown.empty()) {
		_arrowDown = addArrow(arrowDownPos, arrowDown, -1);
	}
	if (!arrowUp.empty()) {
		_arrowUp = addArrow(arrowUpPos, arrowUp, 1);

	}

}

Node* ItemList::addArrow(glm::vec3 pos, const std::string& model, int dir) {

	auto arrow = std::make_shared<Node>();
	arrow->setPosition(pos.x, pos.y, pos.z);
	auto m = AssetManager::instance().getSprite(model);
	arrow->setModel(m, pybind11::dict("batch"_a=_batch));
	arrow->setPalette(_arrowPalUnselected);
	auto modelBounds = m->getBounds();
	auto shape = std::make_shared<AABB>(modelBounds.min.x, modelBounds.max.x, modelBounds.min.y, modelBounds.max.y);
	//auto arrowHotSpot = std::make_shared<HotSpot>(shape, pybind11::dict("batch"_a="ui_line"));
	//arrowHotSpot->set
	arrow->addComponent(std::make_shared<ArrowHotSpot>(shape, _arrowPalUnselected, _arrowPalSelected, dir, pybind11::dict("batch"_a="ui_line")));


	add(arrow);
	return arrow.get();
}

void ItemList::clear() {
	for (const auto& n : _items) {
		n.node->remove();
	}
	_itemCount = 0;
	_nextRow = 0;
	_rowSelected=0;
	_rowTop=0;
	_items.clear();
	updateIndices();

}

void ItemList::start() {
	Node::start();
	updateIndices();
}

void ItemList::up() {
	if (_firstVisibleItem == 0) {
		// edge case. do nothing
		return;
	}
	const auto& item = _items[_firstVisibleItem-1];
	_rowTop = item.rowId;
//
//
//	if (_rowSelected == 0) {
//		return;
//	}
//	if (_rowSelected == _rowTop) {
//		_rowTop -= 1;
//	}
//	_rowSelected -= 1;
	updateIndices();
}

void ItemList::select() {
	if (_callback) {
		//_callback(_rowToText.at(_rowSelected));
	}
}

void ItemList::down() {
	if (_lastVisibleItem == _items.size() - 1) {
		return;
	}
	const auto& item = _items[_firstVisibleItem+1];
	_rowTop = item.rowId;

//	if (_rowSelected >= _itemCount - 1) {
//		return;
//	}
//	if (_rowSelected == _rowTop + _height-1) {
//		_rowTop += 1;
//	}
//	_rowSelected += 1;
	updateIndices();
}

void ItemList::updateIndices() {
	if (!m_active) return;
	//_cursor->setPosition(0.f, -(_rowSelected-_rowTop)*_lineHeight, 0.f);
	_firstVisibleItem = -1;
	_lastVisibleItem = -1;
	int id{0};
	for (const auto& item : _items) {
		auto notVisible = (item.rowId + item.rows - 1 < _rowTop || item.rowId >= _rowTop + _height);
		if (!notVisible) {
			if (_firstVisibleItem == -1) {
				_firstVisibleItem = id;
			}
			_lastVisibleItem = id;
		}
		item.node->setActive(!notVisible);
		item.node->setPosition(8.f, -(item.rowId - _rowTop)*_lineHeight, 0.f);
		id ++;
	}


	if (_arrowDown != nullptr) {
		bool arrowDownVisible = _lastVisibleItem < _items.size() - 1;
		_arrowDown->setActive(arrowDownVisible);
	}
	if (_arrowUp != nullptr) {
		bool arrowUpVisible = _firstVisibleItem > 0;
		_arrowUp->setActive(arrowUpVisible);
	}
}

void ItemList::addItem(const pybind11::kwargs& kwargs) {

	pybind11::kwargs args;
	pybind11::kwargs args1;
	auto text = py_get_dict<std::string>(kwargs, "text");
	auto obj = py_get_dict<pybind11::object>(kwargs, "user_data", pybind11::object());
	args["font"] = _font;
	args["text"] = text;
	args1["batch"] = _batch;
	args["line_height"] = _lineHeight;
	auto node = std::make_shared<Node>();
	auto model = std::make_shared<Text>(args);
	Item item;
	item.rows = model->getLines();
	item.rowId = _nextRow;
	item.text = text;
	item.node = node.get();
	_nextRow += item.rows;
	node->setModel(model, args1);
	node->setPosition(_offsetx, -_nextRow * _lineHeight, 0.f);
	node->setPalette(_palId);
	if (_useMouse) {
		auto textHotspot = std::make_shared<TextHotSpot>(pybind11::dict("batch"_a = "ui_line"));
		if (_onEnter) {
			textHotspot->setOnEnter(_onEnter);
		}
		if (_onLeave) {
			textHotspot->setOnLeave(_onLeave);
		}
		if (_onClick) {
			textHotspot->setOnClick(_onClick);
		}
		node->addComponent(textHotspot);
	}
	if (obj) {
		node->setUserData(obj);
	}
	add(node);
	_items.push_back(item);
	//_rowToId[_itemCount] = node->getId();
	//_rowToText[_itemCount] = text;
	_itemCount++;
	updateIndices();

}

int ItemListKeyListener::keyCallback(GLFWwindow *, int key, int scancode, int action, int mods) {

	if (action == GLFW_PRESS || action == GLFW_REPEAT) {
		switch (key) {
			case GLFW_KEY_UP:
				_li->up();
				break;
			case GLFW_KEY_DOWN:
				_li->down();
				break;
			case GLFW_KEY_ENTER:
				if (action != GLFW_REPEAT) {
					_li->select();
				}
				break;
		}
	}
	return 0;
}