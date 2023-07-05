#include "batch.h"
#include "components/renderer.h"


void IBatch::add(Node * node, const pybind11::kwargs& args) {
    auto model = node->getModel();
    auto renderer = model->getRenderer(this);
    node->addComponent(renderer);
	renderer->setModel(model, args);

}

int IBatch::getPrimitiveId() {
    int next;
    if (_deallocated.empty()) {
        next = _nPrimitive;
        _nPrimitive++;
    } else {
        next = _deallocated.front();
        _deallocated.pop_front();
    }
    return next;
}

void IBatch::release(int id) {
    _deallocated.push_back(id);

}