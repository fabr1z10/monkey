#pragma once

#include "../runners/scheduler.h"

class NodeAction : public Action {
public:
	explicit NodeAction(int id);
	void start() override;
protected:
	Node* m_node;
	int m_nodeId;
	//std::string _tag;
};
