#pragma once

#include "../runners/scheduler.h"

class NodeAction : public Action {
public:
	explicit NodeAction(const pybind11::kwargs&);
	void start() override;
protected:
	Node* m_node;
	int m_nodeId;
};
