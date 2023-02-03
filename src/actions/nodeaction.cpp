#include "nodeaction.h"
#include "../engine.h"

NodeAction::NodeAction(const pybind11::kwargs& args) {
	m_node = nullptr;
	if (args.contains("id")) {
		m_nodeId = args["id"].cast<int>();
	} else {
		m_node = args["node"].cast<std::shared_ptr<Node>>().get();
	}
}

void NodeAction::start() {
	Action::start();
	if (m_node == nullptr) {
		m_node = Engine::instance().getNode(m_nodeId).get();
	}
}