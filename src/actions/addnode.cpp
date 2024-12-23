#include "addnode.h"

#include <utility>


AddNode::AddNode(int id, std::shared_ptr<Node> node, const pybind11::kwargs& args) : NodeAction(id, args), _nodeToAdd(std::move(node)) {

}

void AddNode::start() {
	NodeAction::start();
	m_node->add(_nodeToAdd);
	_nodeToAdd = nullptr;
}

int AddNode::process(double) {
	return 0;
}