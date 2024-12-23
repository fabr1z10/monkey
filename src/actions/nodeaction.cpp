#include "nodeaction.h"
#include "../engine.h"
#include "../pyhelper.h"


NodeAction::NodeAction(int id, const pybind11::kwargs& args) : Action(args), m_nodeId(id), m_node(nullptr) {

}

void NodeAction::setNode(Node *node) {
	m_node = node;
	_id = node->getId();
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

void NodeAction::onEnd() {
	if (_onEnd) {
		_onEnd(m_node);
	}
}