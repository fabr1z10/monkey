#include "walk.h"
#include "../node.h"

Walk::Walk(int id, glm::vec2 target, const pybind11::kwargs& args) : NodeAction(id, args), _target(target) {
}

WalkDynamic::WalkDynamic(int id, pybind11::function f, const pybind11::kwargs& args) : NodeAction(id, args), _func(f) {
}


void Walk::start() {
    NodeAction::start();
    _char = m_node->getComponent<WalkableCharacter>();
    assert(_char != nullptr);
    _char->goTo(_target);
}

void WalkDynamic::start() {
    NodeAction::start();
    _char = m_node->getComponent<WalkableCharacter>();
    assert(_char != nullptr);
    glm::vec2 target= _func().cast<glm::vec2>();
    _char->goTo(target);

}

int Walk::process(double) {
    return (_char->moving() ? 1 : 0);
}

int WalkDynamic::process(double) {
    return (_char->moving() ? 1 : 0);
}


