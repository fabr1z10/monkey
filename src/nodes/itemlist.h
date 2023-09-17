#include "../node.h"
#include "../keylistener.h"
#include "../components/hotspot.h"


class ItemList : public Node {
public:
	ItemList(const pybind11::kwargs& args);

	void addItem (const pybind11::kwargs& args);
	void clear();
	void start() override;
	void down();
	void up();
	void select();
private:
	Node* addArrow(glm::vec3 pos, const std::string& model, int);
	struct Item {
		int itemId;
		int rowId;
		int rows;
		std::string text;
		Node* node;
	};
	void updateIndices();
	int _rowSelected;
	int _rowTop;
	int _height;
	int _nextRow;
	bool _useMouse;
	std::string _font;
	float _lineHeight;
	int _itemCount;
	float _offsetx;
	int _palId;
	std::string _batch;
	//std::unordered_map<int, long> _rowToId;
	//std::unordered_map<int, std::string> _rowToText;
	std::vector<Item> _items;
	std::unique_ptr<KeyboardListener> _listener;
	Node* _cursor;
	pybind11::function _callback;
	pybind11::function _onEnter;
	pybind11::function _onLeave;
	pybind11::function _onClick;
	int _arrowPalUnselected;
	int _arrowPalSelected;
	int _firstVisibleItem;
	int _lastVisibleItem;
	Node* _arrowUp;
	Node* _arrowDown;
};

class ItemListKeyListener : public KeyboardListener {
public:
	ItemListKeyListener(ItemList* li) : KeyboardListener(), _li(li) {}
	void keyCallback(GLFWwindow*, int key, int scancode, int action, int mods) override;
private:
	ItemList* _li;
};


class ArrowHotSpot : public HotSpot {
public:
	ArrowHotSpot(std::shared_ptr<Shape> shape, int palUnselected, int palSelected, int dir, const pybind11::kwargs& args);
	void start() override;
	void onEnter () override;
	void onLeave () override;
	void onClick (glm::vec2 pos, int button, int action) override;
private:
	int _palUnselected;
	int _palSelected;
	ItemList* _itemList;
	int _dir;
};
