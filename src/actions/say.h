#pragma once

#include "nodeaction.h"
#include <glm/glm.hpp>
#include "../components/scummcharacter.h"


class Say : public NodeAction {
public:
	Say(const pybind11::kwargs&);
	void start() override;
	int run(double) override;
	void end() override;
private:
	std::string _text;
	std::string _fontId;
	std::string _batchId;
	float _timeout;
	ScummCharacter* _sc;
	glm::vec2 _margin;
	float _offset;
	double _timer;
	Node* _textNode;
};