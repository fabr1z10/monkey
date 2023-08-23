#include "nodeaction.h"
#include "../engine.h"
#include "../pyhelper.h"


NodeAction::NodeAction(const pybind11::kwargs& args) {
	m_node = nullptr;
	if (args.contains("id")) {
		m_nodeId = args["id"].cast<int>();
	} else {
		// TAG
		_tag = py_get_dict<std::string>(args, "tag", "");
		if (_tag.empty()) {
			m_node = args["node"].cast<std::shared_ptr<Node>>().get();
		}
	}
}

void NodeAction::start() {
	Action::start();
	if (m_node == nullptr) {
		if (_tag.empty()) {
			m_node = Engine::instance().getNode(m_nodeId);
		} else {
			m_node = (*Engine::instance().getNodes(_tag).begin());
		}
	}
}