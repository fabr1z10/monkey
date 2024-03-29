#pragma once

#include <typeindex>

class Node;

class Component {
public:
    Component();
    virtual ~Component() = default;
    virtual std::type_index getType();
    virtual void start() {}
    virtual void update(double) {}
    virtual void setNode(Node*);
    Node* getNode();
    bool isActive() const;
    virtual void setActive(bool);
	//using Base = Component;
protected:
    // the node this component belongs to
    Node* m_node;
    bool m_active;
};

inline Node* Component::getNode() {
    return m_node;
}

inline void Component::setNode(Node* node) {
    m_node = node;
}




