#pragma once

#include "nodeaction.h"
#include "../components/sprite_renderer.h"

class AddNode : public NodeAction {
public:
	AddNode(int id, std::shared_ptr<Node>);
	void start() override;
	int process(double) override;
private:
	std::shared_ptr<Node> _nodeToAdd;
};
