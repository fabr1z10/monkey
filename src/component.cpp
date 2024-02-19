#include "component.h"

Component::Component() : m_node(nullptr) {}

std::type_index Component::getType() {
    return std::type_index(typeid(*this));
}
//
//bool Component::isActive() const {
//    return m_active;
//}
//
void Component::setState(NodeState state) {
   _state = state;
}