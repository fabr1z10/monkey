#include "batch.h"
#include "components/renderer.h"


void IBatch::add(Node * node, const pybind11::kwargs& args) {
    auto model = node->getModel();
    auto renderer = model->getRenderer(this);
    renderer->setModel(model, args);
    node->addComponent(renderer);

}