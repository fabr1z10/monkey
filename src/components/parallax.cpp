#include "parallax.h"
#include "../node.h"
#include "../engine.h"


Parallax::Parallax(int camId, float z, glm::vec2 basePosition, glm::vec2 parallaxFactor, const pybind11::kwargs& args) : Component(args), _camId(camId),
	_parallaxFactor(parallaxFactor), _basePosition(basePosition), _z(z) {
}

void Parallax::start() {
	_cam = dynamic_cast<OrthoCamera*>(Engine::instance().getRoom()->getCamera(_camId));
	auto size = _cam->getSize();
	_camBasePos.x = size.x * 0.5f;
	_camBasePos.y = size.y * 0.5f;

}

void Parallax::update(double) {
	auto pos = _cam->getPosition();
	auto itemPos = _basePosition + glm::vec2( (pos.x - _camBasePos.x) * (1.f - _parallaxFactor.x),
										   (pos.y - _camBasePos.y) * (1.f - _parallaxFactor.y));
	m_node->setPosition(itemPos.x, itemPos.y, _z);
}