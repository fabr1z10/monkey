#include "walk.h"
#include "../engine.h"
#include "../nodes/walkarea.h"
#include "../pyhelper.h"
#include <iostream>

Walk::Walk(const pybind11::kwargs & args) : NodeAction(args) {
	_target = py_get_dict<glm::vec2>(args, "target");


}

void Walk::start() {
	NodeAction::start();
	auto walkarea = dynamic_cast<WalkArea*>(m_node->getParent());
	if (walkarea) {
		std::cout << "ciao\n";
		// find closest point
		std::cout << " === called walk with (" <<_target.x << ", " << _target.y << ")\n";

		//std::cout << " target: " << actualTarget.x << ", " << actualTarget.y << "\n";
		glm::vec2 startPosition = m_node->getWorldPosition();
		auto path = walkarea->getPath(startPosition, _target);

		std::cout << "---\n";
		for (const auto& p : path) {
			std::cout << p.x << ", " << p.y << "\n";
		}
		std::cout << "---\n";
	}


}