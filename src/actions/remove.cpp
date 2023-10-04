#include "remove.h"
#include "../node.h"

int RemoveNode::process(double) {
	m_node->remove();
	return 0;
}