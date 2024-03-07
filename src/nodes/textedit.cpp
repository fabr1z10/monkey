#include "textedit.h"
#include "../pyhelper.h"
#include <GLFW/glfw3.h>

using namespace pybind11::literals; // to bring in the `_a` literal

TextEdit::TextEdit(const std::string& batchId, const std::string& font, const std::string& prompt, const std::string& cursor,
				   const pybind11::kwargs& args) :
	Text(batchId, font, prompt + cursor, args), _prompt(prompt), _cursor(cursor) {

	_listener = std::make_unique<TextEditListener>(this, prompt, cursor);
	_onEnter = py_get_dict<pybind11::function>(args, "on_enter", pybind11::function());
	//setModel(textModel, pybind11::dict("batch"_a=batch));
}

TextEdit::TextEditListener::TextEditListener(TextEdit* node, const std::string & prompt, const std::string& cursor) :
		_node(node), _prompt(prompt), _cursor(cursor) {
}

void TextEdit::sendMessage(const std::string & text) {
	if (_onEnter) {
		_onEnter(text);
	}

}

int TextEdit::TextEditListener::keyCallback(GLFWwindow *, int key, int scancode, int action, int mods) {

    if (_node->getState() != NodeState::ACTIVE) {
        return 0;
    }
	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_ENTER) {
			_node->sendMessage(_text);
			_text.clear();
		} else if (key == GLFW_KEY_BACKSPACE) {
			if (!_text.empty()) _text.pop_back();
		} else if (key == GLFW_KEY_SPACE) {
			_text += " ";
		} else {
			if (key >= GLFW_KEY_A && key <= GLFW_KEY_Z) {
				auto ciao = (unsigned int) ((mods & GLFW_MOD_SHIFT) ? L'A' : L'a');
				ciao += (key - GLFW_KEY_A);
				char g = ciao;
				_text += g;
			} else {
			    return 0;
			}
		}
        _node->updateText(_prompt + _text + _cursor);

		//_node->setModel(_model, pybind11::dict("batch"_a="ui"));
	}
}