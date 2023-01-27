#pragma once

#include <memory>
#include <unordered_map>
#include <pybind11/pybind11.h>
#include "glm/glm.hpp"
#include "camera.h"
#include "component.h"
#include "model.h"
#include "event.h"

class Node {
public:
    Node();
    ~Node();
    long getId() const;
    Node* getParent();
    void setParent(Node*);
    void add(std::shared_ptr<Node>);
    void moveTo(std::shared_ptr<Node> node);
    void removeChild(long);
    // remove this node
    void remove();

    // node lifetime
    void start();
    void update(double) ;
    const std::unordered_map<long, std::shared_ptr<Node>>& getChildren() const;

    void setActive(bool);
    bool active() const;

	bool getFlipX() const;
	void setFlipX(bool value);

    glm::mat4 getWorldMatrix() const;
	glm::vec3 getWorldPosition() const;

	std::shared_ptr<Camera> getCamera();
    void setCamera(std::shared_ptr<Camera>);
    pybind11::object getUserData();
    void setUserData(pybind11::object);
    void setPosition(float, float, float);
	void move(glm::mat4 m);
	void move(glm::vec3 delta);
	void setModel(std::shared_ptr<Model> model);

    void addComponent(std::shared_ptr<Component> c) ;
    template <typename T>
    T* getComponent() const {
        auto it = m_components.find(std::type_index(typeid(T)));
        if (it != m_components.end()) {
            return dynamic_cast<T*>(it->second.get());
        }
        return nullptr;
    }

    // --- events ---
	Event<Node*> onMove;						// fires when this node moves
	Event<Node*> onRemove;                      // fires when node is deleted

private:
	void notifyMove();
    long _id;
    Node* m_parent;
    std::unordered_map<long, std::shared_ptr<Node>> m_children;
    std::unordered_map<std::type_index, std::shared_ptr<Component> > m_components;
    glm::mat4 m_modelMatrix;
    glm::mat4 m_worldMatrix;
    glm::mat4 m_scaleMatrix;
    bool m_active;
    bool m_started;
    pybind11::object m_userData;
    std::shared_ptr<Camera> m_camera;

};

inline long Node::getId() const {
    return _id;
}

inline Node* Node::getParent() {
    return m_parent;
}


inline const std::unordered_map<long, std::shared_ptr<Node>> & Node::getChildren() const {
    return m_children;
}

inline bool Node::active() const {
    return m_active;
}

inline std::shared_ptr<Camera> Node::getCamera() {
    return m_camera;
}