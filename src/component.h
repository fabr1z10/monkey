#pragma once

#include <typeindex>
#include <pybind11/pybind11.h>


class Node;

enum NodeState {
    ACTIVE = 0,
    PAUSED = 1,
    INACTIVE = 2
};

class Component {
public:
    Component(const pybind11::kwargs& args);
    virtual ~Component() = default;
    virtual std::type_index getType();
    virtual void start() {}
    virtual void update(double) {}
    virtual void setNode(Node*);
    Node* getNode();
    virtual void sendMessage(const pybind11::kwargs& args) {}
    virtual void setState(NodeState state);
    NodeState getState() const;
	//using Base = Component;
	std::string getLabel() const;
protected:
    // the node this component belongs to
    Node* m_node;
    NodeState _state;
    std::string _label;

};

inline std::string Component::getLabel() const {
    return _label;
}

inline Node* Component::getNode() {
    return m_node;
}

inline void Component::setNode(Node* node) {
    m_node = node;
}

inline NodeState Component::getState() const {
    return _state;
}



