#include "batch.h"
#include "components/renderer.h"
#include "pyhelper.h"


IBatch::IBatch(int verticesPerElement, const pybind11::kwargs& args) : _vertsPerElement(verticesPerElement), _nPrimitive(0) {
	_maxElements = py_get_dict<int>(args, "max_elements");
	_camId = py_get_dict<int>(args, "cam");
}

//void IBatch::add(Node * node, const pybind11::kwargs& args) {
//    auto model = node->getModel();
//    auto renderer = model->getRenderer(this);
//    node->addComponent(renderer);
//	renderer->setNodeModel(model, args);
//
//}

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