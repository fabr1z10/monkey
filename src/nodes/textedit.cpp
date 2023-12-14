#include "textedit.h"
#include "../pyhelper.h"
#include <GLFW/glfw3.h>

using namespace pybind11::literals; // to bring in the `_a` literal

TextEdit::TextEdit(const pybind11::kwargs &args) : Node() {
	auto font = py_get_dict<std::string>(args, "font");
	auto prompt = py_get_dict<std::string>(args, "prompt");
	auto cursor = py_get_dict<std::string>(args, "cursor");
	auto batch = py_get_dict<std::string>(args, "batch");
	auto textModel = std::make_shared<Text>(pybind11::dict("text"_a = prompt + cursor, "font"_a = font));
	_listener = std::make_unique<TextEditListener>(this, textModel, prompt, cursor);
	_onEnter = py_get_dict<pybind11::function>(args, "on_enter", pybind11::function());
	setModel(textModel, pybind11::dict("batch"_a=batch));
}

TextEdit::TextEditListener::TextEditListener(TextEdit* node, std::shared_ptr<Text> text, const std::string & prompt, const std::string& cursor) :
	_node(node), _model(text), _prompt(prompt), _cursor(cursor) {
}

void TextEdit::sendMessage(const std::string & text) {
    if (_onEnter) {
        _onEnter(text);
    }

}

int TextEdit::TextEditListener::keyCallback(GLFWwindow *, int key, int scancode, int action, int mods) {
	if (!_node->active()) return 0;
	if (action == GLFW_PRESS) {
	    if (key == GLFW_KEY_ENTER) {
            _node->sendMessage(_text);
	        _text.clear();
	    }
		if (key == GLFW_KEY_BACKSPACE) {
            if (!_text.empty()) _text.pop_back();
        } else if (key == GLFW_KEY_SPACE) {
		    _text += " ";
		} else {
			if (key >= GLFW_KEY_A && key <= GLFW_KEY_Z) {
				auto ciao = (unsigned int) ((mods & GLFW_MOD_SHIFT) ? L'A' : L'a');
				ciao += (key - GLFW_KEY_A);
				char g = ciao;
				_text += g;

			}
		}		_model->setText(_prompt + _text + _cursor);
		_node->setModel(_model, pybind11::dict("batch"_a="ui"));
	}
}