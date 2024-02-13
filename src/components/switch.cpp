#include "switch.h"

Switch::Switch() : Component(), _activeComponent(-1) {

}

void Switch::start() {

    for (auto c : _components) {
        c->setNode(m_node);
        c->start();
    }

}

void Switch::add(std::shared_ptr<Component> c) {
    _components.push_back(c);
}

void Switch::update(double dt) {
    if (_activeComponent != -1) {
        _components[_activeComponent]->update(dt);
    }

}

void Switch::enable(int id) {
    _activeComponent = id;
}