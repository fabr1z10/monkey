#pragma once

#include "nodeaction.h"
#include <glm/glm.hpp>
#include "../components/scummcharacter.h"


class Turn : public NodeAction {
public:
	Turn(const pybind11::kwargs&);
	void start() override;
	int run(double) override;
private:
	glm::vec2 _dir;
	ScummCharacter* _sc;
	static std::unordered_map<std::string, glm::vec2> m;
};