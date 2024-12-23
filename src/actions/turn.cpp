#include "turn.h"
#include "../components/renderer.h"
#include "../node.h"

Turn::Turn(int id, const std::string& dir) : NodeAction(id, pybind11::dict()), _direction(dir) {
}


void Turn::start() {
    NodeAction::start();
    m_node->getComponent<WalkableCharacter>()->setDirection(_direction);
//    auto renderer = m_node->getComponent<Renderer>();
//    switch (_direction[0]) {
//        case 'n':
//            renderer->setAnimation("idle_n");
//            m_node->setFlipX(false);
//            break;
//        case 's':
//            renderer->setAnimation("idle_s");
//            m_node->setFlipX(false);
//            break;
//        case 'e':
//            renderer->setAnimation("idle_e");
//            m_node->setFlipX(false);
//            break;
//        case 'w':
//            renderer->setAnimation("idle_e");
//            m_node->setFlipX(true);
//            break;
//    }
}
int Turn::process(double) {
     return 0;
}
