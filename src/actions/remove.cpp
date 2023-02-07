#include "remove.h"
#include "../node.h"

int RemoveNode::run(double) {
	m_node->remove();
	return 0;
}