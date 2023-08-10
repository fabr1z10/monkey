#include "../node.h"
#include "../keylistener.h"



class ItemList : public Node {
public:
	ItemList(const pybind11::kwargs& args);

	void addItem (const std::string& text);
	void clear();
	void start() override;
	void down();
	void up();
	void select();
private:
	void updateIndices();
	int _rowSelected;
	int _rowTop;
	int _height;
	std::string _font;
	float _lineHeight;
	int _itemCount;
	float _offsetx;
	std::unordered_map<int, long> _rowToId;
	std::unordered_map<int, std::string> _rowToText;
	std::unique_ptr<KeyboardListener> _listener;
	Node* _cursor;
	pybind11::function _callback;
};

class ItemListKeyListener : public KeyboardListener {
public:
	ItemListKeyListener(ItemList* li) : KeyboardListener(), _li(li) {}
	void keyCallback(GLFWwindow*, int key, int scancode, int action, int mods) override;
private:
	ItemList* _li;
};
