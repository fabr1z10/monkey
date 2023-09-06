#include "say.h"
#include "../pyhelper.h"
#include "../node.h"
#include "../components/scummcharacter.h"
#include "../models/text.h"
#include "../engine.h"

using namespace pybind11::literals; // to bring in the `_a` literal

Say::Say(const pybind11::kwargs & args) : NodeAction(args) {
	_text = py_get_dict<std::string>(args, "text");
	// this could be globals...
	auto settings = Engine::instance().getConfig();
	_fontId = py_get_dict<std::string>(args, "font", py_get<std::string>(settings, "dialogue_font"));
	_batchId = py_get_dict<std::string>(args, "batch", py_get<std::string>(settings, "dialogue_batch"));
	_margin = py_get_dict<glm::vec2>(args, "margin", py_get<glm::vec2>(settings, "dialogue_margin"));
	_offset = py_get_dict<float>(args, "offset", py_get<float>(settings, "dialogue_offset"));
	_timeout = py_get_dict<float>(args, "timeout", py_get<float>(settings, "dialogue_timeout"));
}


void Say::start() {
	NodeAction::start();
	_sc = m_node->getComponent<ScummCharacter>();
	auto pal = _sc->getTextPalette();

	auto actorPosition = m_node->getWorldPosition();
	auto textNode = std::make_shared<Node>();

	auto textModel = std::make_shared<Text>(pybind11::dict("text"_a = _text,
		"font"_a = _fontId, "halign"_a=1));
	textNode->setModel(textModel, pybind11::dict("batch"_a = _batchId));
	auto size = textModel->getBounds().getSize();
	auto cameraId = Engine::instance().getRoom()->getBatch(_batchId)->getCameraId();
	auto view_bounds = Engine::instance().getRoom()->getCamera(cameraId)->getViewingBounds();

	float x = std::max(view_bounds.min.x + _margin.x + size.x * 0.5f, actorPosition.x);
	float y = std::min(view_bounds.max.y - _margin.y - size.y, actorPosition.y + _offset);

	textNode->setPosition(x, y, 1.f);
	textNode->setPalette(pal);
	m_node->getParent()->getParent()->add(textNode);
	_timer = 0.0;
	_textNode = textNode.get();

}

int Say::run(double dt) {
	_timer += dt;
	if (_timer >= _timeout) {
		_textNode->remove();
		return 0;

	}
	return 1;
}