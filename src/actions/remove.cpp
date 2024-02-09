#include "remove.h"
#include "../node.h"


RemoveNode::RemoveNode(int id) : NodeAction(id) {

}

void RemoveNode::start() {
    NodeAction::start();
    m_node->remove();
}

int RemoveNode::process(double) {
    return 0;
}