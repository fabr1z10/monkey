#include "remove.h"
#include "../node.h"


RemoveNode::RemoveNode(int id, const pybind11::kwargs& args) : NodeAction(id, args) {

}

void RemoveNode::start() {
    NodeAction::start();
    m_node->remove();
}

int RemoveNode::process(double) {
    return 0;
}