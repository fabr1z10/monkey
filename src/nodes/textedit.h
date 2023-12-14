#pragma once

#include "../models/text.h"
#include "../keylistener.h"




class TextEdit : public Node {
public:
	TextEdit(const pybind11::kwargs& args);
    void sendMessage(const std::string&);
private:
	class TextEditListener : public KeyboardListener {
	public:
		TextEditListener(TextEdit* node, std::shared_ptr<Text>, const std::string&, const std::string&);
		int keyCallback(GLFWwindow*, int key, int scancode, int action, int mods) override;
	private:
		std::shared_ptr<Text> _model;
		TextEdit* _node;
		std::string _prompt;
		std::string _cursor;
		std::string _text;
	};

	std::unique_ptr<TextEditListener> _listener;
	pybind11::function _onEnter;

};