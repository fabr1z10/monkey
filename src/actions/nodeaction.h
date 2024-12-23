#pragma once

#include "../runners/scheduler.h"

class NodeAction : public Action {
public:
	explicit NodeAction(int id, const pybind11::kwargs& args);
	void start() override;
	void setNode(Node* node);
	void onEnd() override;
protected:
	Node* m_node;
	int m_nodeId;

};
