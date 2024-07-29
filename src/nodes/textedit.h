#pragma once

#include "text.h"
#include "../keylistener.h"




class TextEdit : public Text {
public:
	TextEdit(const std::string& batchId, const std::string& font, const std::string& prompt,
		  const std::string& cursor, const pybind11::kwargs& args);
	void sendMessage(const std::string&);
	void setText(const std::string&);
private:
	std::string _cursor;
	std::string _prompt;

	class TextEditListener : public KeyboardListener {
	public:
		TextEditListener(TextEdit* node, const std::string&, const std::string&);
		int keyCallback(GLFWwindow*, int key, int scancode, int action, int mods) override;
		void setText(const std::string& text) {
			_text = text;
		}
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