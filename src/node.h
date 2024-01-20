#pragma once

#include <memory>
#include <unordered_map>
#include <pybind11/pybind11.h>
#include "glm/glm.hpp"
//#include "camera.h"
#include "component.h"
#include "model.h"
#include "event.h"

class Node {
public:
    Node();
    Node(const Node&);
    ~Node();
    virtual std::shared_ptr<Node> clone();
    long getId() const;
    Node* getParent();
    void setParent(Node*);
    //std::string getAnimation() const;
    //virtual void setAnimation(const std::string&);

    virtual void add(std::shared_ptr<Node>);
    void moveTo(std::shared_ptr<Node> node);
    void removeChild(long);
    // remove this node
    void remove();

    // node lifetime
    virtual void start();
    void startRecursive();
    void update(double) ;
    virtual void postProcess() {}
    const std::unordered_map<long, std::shared_ptr<Node>>& getChildren() const;

    void setActive(bool);
    bool active() const;

	bool getFlipX() const;
	void setFlipX(bool value);

    glm::mat4 getWorldMatrix() const;
	glm::vec3 getLocalPosition() const;
    glm::vec3 getWorldPosition() const;

	//std::shared_ptr<Camera> getCamera();
    //void setCamera(std::shared_ptr<Camera>);
    pybind11::object getUserData();
    void setUserData(pybind11::object);
    virtual void setPosition(float, float, float);
	void move(glm::mat4 m);
	void move(glm::vec3 delta);
	void rotate(float angle, glm::vec3 axis);
	void movea(glm::vec3 delta);
	std::shared_ptr<Model> getModel();
	virtual void setModel(std::shared_ptr<Model> model, const pybind11::kwargs& args = pybind11::kwargs());


	Bounds getBounds();
//	std::string getState() const;
//	void setState (const std::string& state, const pybind11::kwargs&);
    void addComponent(std::shared_ptr<Component> c) ;
    float getX() const;
    float getY() const;
	float getZ() const;
	void setZ(float);
    template <typename T>
    T* getComponent() const {
        auto it = m_components.find(std::type_index(typeid(typename T::Base)));
        if (it != m_components.end()) {
            return dynamic_cast<T*>(it->second.get());
        }
        return nullptr;
    }

    // --- events ---
	Event<Node*> onMove;						// fires when this node moves
	Event<Node*> onRemove;                      // fires when node is deleted
	void setTag(const std::string&);
	std::string getTag() const;
	//std::string getText() const;
	//void setText(const std::string&);
	void setScale(float);
	float getScale() const;
protected:
	void notifyMove();
    long _id;
	float _scale;
    Node* m_parent;
    std::unordered_map<long, std::shared_ptr<Node>> m_children;
    std::unordered_map<std::type_index, std::shared_ptr<Component> > m_components;
    glm::mat4 m_modelMatrix;
    glm::mat4 m_worldMatrix;
    glm::mat4 m_scaleMatrix;
    bool m_active;
    bool m_started;
    pybind11::object m_userData;
    //std::shared_ptr<Camera> m_camera;
	std::shared_ptr<Model> m_model;
protected:
	std::string _tag;

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

