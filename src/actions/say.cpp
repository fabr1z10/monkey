#include "say.h"
#include "../pyhelper.h"
#include "../node.h"
#include "../components/scummcharacter.h"
#include "../models/text.h"
#include "../engine.h"

using namespace pybind11::literals; // to bring in the `_a` literal


ShowMessageBase::MessageKeyListener::MessageKeyListener(Action * action) : KeyboardListener(), _action(action) {

}

void ShowMessageBase::MessageKeyListener::addKey(int key) {
	_keys.insert(key);
}
int ShowMessageBase::MessageKeyListener::keyCallback(GLFWwindow *, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS && _keys.count(key) > 0) {
		_action->stop();
		return 1;
	}
	return 0;
}

ShowMessageBase::ShowMessageBase(const std::string& font, const std::string& text, const std::string& batchId,
	glm::vec3 position, int palette, float timeOut, glm::vec2 margin, int parentId, float maxWidth, int hAlign,
	int vAlign, const pybind11::kwargs& kw) : Action(), _text(text), _fontId(font), _batchId(batchId),
	_timeout(timeOut), _margin(margin), _hAlign(hAlign), _vAlign(vAlign), _width(maxWidth),
	_position(position), _palette(palette), _timer(0.f), _textNode(nullptr) {
	//_text = py_get_dict<std::string>(args, "text");
	//auto settings = Engine::instance().getConfig();
	//_fontId = py_get_dict<std::string>(args, "font", py_get<std::string>(settings, "dialogue_font"));
	//_batchId = py_get_dict<std::string>(args, "batch", py_get<std::string>(settings, "dialogue_batch"));
	//_timeout = py_get_dict<float>(args, "timeout", 0.f);
	//_margin = py_get_dict<glm::vec2>(args, "margin", py_get<glm::vec2>(settings, "dialogue_margin"));

	//auto parentId = py_get_dict<int>(args, "parent", Engine::instance().getRoom()->getRoot()->getId());
	_msgParentNode = Engine::instance().getNode(parentId);
	//_width = py_get_dict<float>(args, "width", py_get<float>(settings, "dialogue_width"));
	//_removeEvents = py_get_dict<int>(args, "remove", 0);
	// halign: 0 = left, 1 = center, 2 = right
	//_hAlign = py_get_dict<int>(args, "halign", 0);
	// valign: 0 = top, 1 = center, 2 = bottom
	//_vAlign = py_get_dict<int>(args, "valign", 0);
	_onCreate = py_get_dict<pybind11::function>(kw, "on_create", pybind11::function());

}

void ShowMessageBase::stop() {
	Action::stop();
	_textNode->remove();
}

int ShowMessageBase::process(double dt) {
	if (_timeout > 0.f) {
		_timer += dt;
		if (_timer >= _timeout) {
			_textNode->remove();
			return 0;
		}
	}
	return 1;
}

void ShowMessageBase::onEnd() {
	if (_textNode) {
		_textNode->remove();
	}
}


Say::Say(const std::string& font, const std::string& text, const std::string& batchId, glm::vec3 pos,
		 int palette, float timeOut, glm::vec2 margin, int parentId, float maxWidth, int hAlign, int vAlign, const pybind11::kwargs& kwargs) :
		ShowMessageBase(font, text, batchId, pos, palette, timeOut, margin, parentId, maxWidth, hAlign, vAlign, kwargs) {

	// this could be globals...
	auto settings = Engine::instance().getConfig();

	//_offset = py_get_dict<float>(args, "offset", py_get<float>(settings, "dialogue_offset"));
}

void ShowMessageBase::createTextNode(float x, float y, unsigned pal) {
	_textNode = nullptr;
	auto textNode = std::make_shared<Node>();
	auto textModel = std::make_shared<Text>(pybind11::dict("text"_a = _text,
														   "font"_a = _fontId, "halign"_a=0, "width"_a=_width, "pal"_a=pal));
	textNode->setModel(textModel, pybind11::dict("batch"_a = _batchId));
	auto size = textModel->getBounds().getSize();
	auto cameraId = Engine::instance().getRoom()->getBatch(_batchId)->getCameraId();
	auto view_bounds = Engine::instance().getRoom()->getCamera(cameraId)->getViewingBounds();

	float pos_x = std::max(view_bounds.min.x + _margin.x + size.x * 0.5f, x);
	float pos_y = std::min(view_bounds.max.y - _margin.y - size.y, y);
	switch (_hAlign) {
		case 1:
			x -= 0.5f * size.x;
			break;
		case 2:
			x -= size.x;
			break;
	}
	switch (_vAlign) {
		case 1:
			y += 0.5f * size.y;
			break;
		case 2:
			y += size.y;
			break;
	}

	textNode->setPosition(x, y, 1.f);
	//textNode->setPalette(pal);
	_msgParentNode->add(textNode);
	_timer = 0.0;
	_textNode = textNode.get();

	if (_removeEvents & 0x01) {
		// remove on button click
		//auto mouseListener
		_mouseListener = std::make_shared<MouseListenerFunc>(
				[&] (GLFWwindow*, double, double) {},
				[&] (GLFWwindow*, int button, int action, int mods) { if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) stop(); });
	}
	if (_timeout == 0.f) {
		_listener = std::make_unique<MessageKeyListener>(this);
		_listener->addKey(GLFW_KEY_ENTER);
		_listener->addKey(GLFW_KEY_ESCAPE);
	}

	if (_onCreate) {
		_onCreate(_textNode, size.x, size.y);
	}
}



void Say::start() {
	Action::start();
	/// TODO RESTORE
	/*
	  _sc = m_node->getComponent<ScummCharacter>();
	auto pal = _sc->getTextPalette();
	auto actorPosition = m_node->getWorldPosition();
	createTextNode(actorPosition.x, actorPosition.y + _offset, pal);
	 */
	/// END
	//auto textNode = std::make_shared<Node>();

	//auto textModel = std::make_shared<Text>(pybind11::dict("text"_a = _text,
	//	"font"_a = _fontId, "halign"_a=1));
	//textNode->setModel(textModel, pybind11::dict("batch"_a = _batchId));
	//auto size = textModel->getBounds().getSize();
	//auto cameraId = Engine::instance().getRoom()->getBatch(_batchId)->getCameraId();
	//auto view_bounds = Engine::instance().getRoom()->getCamera(cameraId)->getViewingBounds();

	//float x = std::max(view_bounds.min.x + _margin.x + size.x * 0.5f, actorPosition.x);
	//float y = std::min(view_bounds.max.y - _margin.y - size.y, actorPosition.y + _offset);

	//textNode->setPosition(x, y, 1.f);
	//textNode->setPalette(pal);
	//m_node->getParent()->getParent()->add(textNode);

}


void ShowMessage::start() {
	Action::start();

	createTextNode(_position.x, _position.y, _palette);
}


ShowMessage::ShowMessage(const std::string& font, const std::string& text, const std::string& batchId, glm::vec3 pos,
	int palette, float timeOut, glm::vec2 margin, int parentId, float maxWidth, int hAlign, int vAlign, const pybind11::kwargs& kw) :
	ShowMessageBase(font, text, batchId, pos, palette, timeOut, margin, parentId, maxWidth, hAlign, vAlign, kw) {

	//_position = py_get_dict<glm::vec2>(args, "pos");
	//_palette = py_get_dict<unsigned>(args, "pal", 0);
}


void OnClickRemove::mouseButtonCallback(GLFWwindow *, int, int, int) {


}