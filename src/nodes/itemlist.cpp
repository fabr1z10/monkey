#include <iostream>
#include "itemlist.h"
#include "../models/text.h"
#include "../assetmanager.h"
#include <GLFW/glfw3.h>

#include "../pyhelper.h"

ItemList::ItemList(const pybind11::kwargs& inargs) : Node(), _itemCount(0), _rowSelected(0), _rowTop(0) {
	_font = py_get_dict<std::string>(inargs, "font");
	_height = py_get_dict<int>(inargs, "height");
	_callback = py_get_dict<pybind11::function>(inargs, "callback", pybind11::function());


	auto f = AssetManager::instance().getFont(_font);
	_lineHeight = f->getLineHeight();

	auto cursor = std::make_shared<Node>();
	pybind11::kwargs args;
	args["text"] = ">";
	args["font"] = _font;
	cursor->setModel(std::make_shared<Text>(args));
	add(cursor);
	cursor->setPosition(0.f, 0.f, 0.f);
	_offsetx=8;
	_cursor = cursor.get();
	_listener = std::make_unique<ItemListKeyListener>(this);
}

void ItemList::clear() {
	for (const auto& n : _rowToId) {
		removeChild(n.second);
	}
	_itemCount = 0;
	_rowToId.clear();
	_rowToText.clear();
	_rowSelected=0;
	_rowTop=0;
	updateIndices();

}

void ItemList::start() {
	Node::start();
	updateIndices();
}

void ItemList::up() {
	if (_rowSelected == 0) {
		return;
	}
	if (_rowSelected == _rowTop) {
		_rowTop -= 1;
	}
	_rowSelected -= 1;
	updateIndices();
}

void ItemList::select() {
	if (_callback) {
		_callback(_rowToText.at(_rowSelected));
	}
}

void ItemList::down() {
	if (_rowSelected >= _itemCount - 1) {
		return;
	}
	if (_rowSelected == _rowTop + _height-1) {
		_rowTop += 1;
	}
	_rowSelected += 1;
	updateIndices();
}

void ItemList::updateIndices() {
	_cursor->setPosition(0.f, -(_rowSelected-_rowTop)*_lineHeight, 0.f);
	for (const auto& n : _rowToId) {
		auto visible = n.first >= _rowTop && n.first < _rowTop + _height;
		m_children.at(n.second)->setActive(visible);
		m_children.at(n.second)->setPosition(8.f, -(n.first - _rowTop)*_lineHeight, 0.f);
	}

}

void ItemList::addItem(const std::string &text) {

	pybind11::kwargs args;
	pybind11::kwargs args1;
	args["font"] = _font;
	args["text"] = text;
	auto node = std::make_shared<Node>();

	node->setModel(std::make_shared<Text>(args), args1);
	node->setPosition(_offsetx, -_itemCount * _lineHeight, 0.f);
	add(node);
	_rowToId[_itemCount] = node->getId();
	_rowToText[_itemCount] = text;
	_itemCount++;
	updateIndices();

}

void ItemListKeyListener::keyCallback(GLFWwindow *, int key, int scancode, int action, int mods) {

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
}