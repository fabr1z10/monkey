#include "sierrafollow.h"
#include "../../engine.h"


NPCSierraFollow::NPCSierraFollow(int target, const pybind11::kwargs& args) : Sierra2DController(args), _target(nullptr), _targetId(target) {

}

void NPCSierraFollow::start() {
    _target = Engine::instance().getNode(_targetId);
    _walkArea = Engine::instance().getRoom()->getRunner<WalkArea>();
}


void NPCSierraFollow::update(double) {

    // get path from source to target
    _walkArea->findPath(m_node->getWorldPosition(), _target->getWorldPosition());
}