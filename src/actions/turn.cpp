#include "turn.h"
#include "../engine.h"
#include "../pyhelper.h"
#include <iostream>
#include "../node.h"
#include "../error.h"

std::unordered_map<std::string, glm::vec2> Turn::m {
	{ "n", glm::vec2(0.f, 1.f) },
	{ "e", glm::vec2(1.f, 0.f) },
	{ "w", glm::vec2(-1.f, 0.f) },
	{ "s", glm::vec2(0.f, -1.f) },
	{ "ne", glm::vec2(1.f, 1.f) },
	{ "nw", glm::vec2(-1.f, 1.f) },
	{ "se", glm::vec2(1.f, -1.f) },
	{ "sw", glm::vec2(-1.f, -1.f) }
};

Turn::Turn(int id, const std::string& dir) : NodeAction(id) {

	_dir = m.at(dir);
}

void Turn::start() {
	NodeAction::start();
	_sc = m_node->getComponent<ScummCharacter>();
	M_Assert(_sc != nullptr, "a node performing a TURN action must have a ScummCharacter component");
	_sc->setDirection(_dir);
	_sc->setAnimation("idle");

	// init animation

}

int Turn::process(double dt) {
	return 0;
}