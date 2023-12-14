#pragma once

#include "nodeaction.h"
#include <glm/glm.hpp>
#include "../components/scummcharacter.h"
#include "../mouselistener.h"
#include "animate.h"


class ShowMessageBase : public Action {
public:
	ShowMessageBase(const std::string& font, const std::string& text, const std::string& batchId, glm::vec3 pos, int palette = 0,
				 float timeOut = 0.f, glm::vec2 margin = glm::vec2(0.f), int parentId = 0,
				 float maxWidth = 0.f, int hAlign = 0, int vAlign = 0, const pybind11::kwargs& kw=pybind11::kwargs());
	int process(double) override;
	void onEnd() override;
	void stop() override;
protected:
	void createTextNode(float x, float y, unsigned);
	std::string _text;
	std::string _fontId;
	std::string _batchId;
	float _timeout;
	glm::vec2 _margin;
	double _timer;
	Node* _textNode;
	Node* _msgParentNode;
	float _width;
	int _removeEvents;
	std::shared_ptr<MouseListener> _mouseListener;
	int _hAlign;
	int _vAlign;
	pybind11::function _onCreate;
	glm::vec3 _position;
	unsigned _palette;
private:
	class MessageKeyListener : public KeyboardListener {
	public:
		MessageKeyListener(Action*);
		void addKey(int);
		int keyCallback(GLFWwindow*, int key, int scancode, int action, int mods) override;
	private:
		Action* _action;
		std::unordered_set<int> _keys;
	};
	std::unique_ptr<MessageKeyListener> _listener;
};

class Say : public ShowMessageBase {
public:
	Say(const std::string& font, const std::string& text, const std::string& batchId, glm::vec3 pos,
		int palette = 0, float timeOut = 0.f, glm::vec2 margin = glm::vec2(0.f), int parentId = 0,
		float maxWidth = 0.f, int hAlign = 0, int vAlign = 0, const pybind11::kwargs& kw=pybind11::kwargs());
	void start() override;
private:
	ScummCharacter* _sc;
	float _offset;
};

class ShowMessage : public ShowMessageBase {
public:
	ShowMessage(const std::string& font, const std::string& text, const std::string& batchId, glm::vec3 pos,
				int palette = 0, float timeOut = 0.f, glm::vec2 margin = glm::vec2(0.f), int parentId = 0,
				float maxWidth = 0.f, int hAlign = 0, int vAlign = 0, const pybind11::kwargs& kw=pybind11::kwargs());
	void start() override;
private:

};


class OnClickRemove : public MouseListener {
public:
	void cursorPosCallback(GLFWwindow*, double, double) override {}
	void mouseButtonCallback(GLFWwindow*, int, int, int) override;
};