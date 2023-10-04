#pragma once

#include "nodeaction.h"

class RemoveNode : public NodeAction {
public:
	explicit RemoveNode(const pybind11::kwargs& args) : NodeAction(args) {}
	int process(double) override;
};