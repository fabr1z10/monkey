#pragma once

#include "nodeaction.h"

class RemoveNode : public NodeAction {
public:
	explicit RemoveNode(int id) : NodeAction(id) {}
	int process(double) override;
};