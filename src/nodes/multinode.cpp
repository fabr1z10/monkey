#include "multinode.h"


void MultiNode::update(double dt) {
    if (_current >= 0) {
        _nodes[_current]->update(dt);
    }


}

void MultiNode::select(int current) {
    _current = current;
}