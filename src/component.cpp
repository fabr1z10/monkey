#include "component.h"
#include "pyhelper.h"

Component::Component(const pybind11::kwargs& args) : m_node(nullptr) {
    _state = py_get_dict<NodeState>(args, "state", NodeState::ACTIVE);
    _tag = py_get_dict<std::string>(args, "tag", "");

}

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