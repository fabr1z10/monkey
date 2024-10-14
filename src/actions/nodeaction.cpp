#include "nodeaction.h"
#include "../engine.h"
#include "../pyhelper.h"


NodeAction::NodeAction(int id) : Action(), m_nodeId(id), m_node(nullptr) {
	//m_node = nullptr;
	//if (id >= 0) {
//		m_nodeId = id;
//} else {
		// TAG
		//_tag = tag;// py_get_dict<std::string>(args, "tag", "");
		//if (_tag.empty()) {
		//		m_node = py_get_dict<std::shared_ptr<Node>>(args, "node", nullptr).get();
		//}
	//}
}

void NodeAction::start() {
	Action::start();
	if (m_node == nullptr) {
		//if (_tag.empty()) {
		m_node = Engine::instance().getNode(m_nodeId);
		m_node->onRemove.reg([&] (Node* node) {
			m_node = nullptr;
			_status = 2;
		});
		//} else {
		//		m_node = (*Engine::instance().getNodes(_tag).begin());
		//}
	}
}