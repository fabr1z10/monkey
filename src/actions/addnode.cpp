#include "addnode.h"

#include <utility>


AddNode::AddNode(int id, std::shared_ptr<Node> node) : NodeAction(id), _nodeToAdd(std::move(node)) {

}

void AddNode::start() {
	NodeAction::start();
	m_node->add(_nodeToAdd);
	_nodeToAdd = nullptr;
}

int AddNode::process(double) {
	return 0;
}