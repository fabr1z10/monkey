#include "walk.h"
#include "../engine.h"
#include "../nodes/walkarea.h"
#include "../pyhelper.h"
#include <iostream>
#include "../node.h"
#include "../error.h"

Walk::Walk(const pybind11::kwargs & args) : NodeAction(args) {
	_target = py_get_dict<glm::vec2>(args, "target");


}

void Walk::start() {
	NodeAction::start();
	_sc = m_node->getComponent<ScummCharacter>();
	M_Assert(_sc != nullptr, "a node performing a WALK action must have a ScummCharacter component");
	_speed = _sc->getSpeed();
	auto walkarea = dynamic_cast<WalkArea *>(m_node->getParent());
	if (walkarea) {
		std::cout << "ciao\n";
		// find closest point
		std::cout << " === called walk with (" << _target.x << ", " << _target.y << ")\n";

		//std::cout << " target: " << actualTarget.x << ", " << actualTarget.y << "\n";
		glm::vec2 startPosition = m_node->getWorldPosition();
		auto path = walkarea->getPath(startPosition, _target);

		std::cout << "---\n";
		for (size_t i = 1; i < path.size(); ++i) {
			//std::cout << p.x << ", " << p.y << "\n";
			WalkSegment seg;
			glm::vec2 displacement = path[i] - path[i - 1];
			seg.direction = glm::normalize(displacement);
			seg.length = glm::length(displacement);
			seg.endPoint = path[i];
			_segments.push_back(seg);
		}
		std::cout << "---\n";
	}

	_current = 0;
	_currentLength = 0.f;
	if (!_segments.empty()) {
		_sc->setDirection(_segments[0].direction);
		_sc->setAnimation("walk");
	} else {
		std::cerr << "WARNING! No walk\n";
	}

	// init animation

}

int Walk::run(double dt) {
	if (_segments.empty()) {
		return 0;
	}

	const auto& cseg = _segments[_current];

	float dtf = static_cast<float>(dt);
	glm::vec2 delta = cseg.direction * _speed * dtf;
	m_node->movea(glm::vec3(delta, 0.f));


	_currentLength += _speed * dtf;
	if (_currentLength >= cseg.length) {
		m_node->setPosition(cseg.endPoint.x, cseg.endPoint.y, 0.f);
		_current++;
		_currentLength = 0.f;
		if (_current >= _segments.size()) {
			// end and set idle animation
			_sc->setAnimation("idle");
			return 0;
		} else {
			_sc->setDirection(_segments[_current].direction);
			_sc->setAnimation("walk");
			// set walk animation
		}
	}
	return 1;




}