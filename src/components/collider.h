#pragma once

#include "../component.h"
#include "../shape.h"
#include <memory>
#include <pybind11/pybind11.h>


class ICollisionEngine;

class Collider : public Component {
public:
    Collider(int, int, int, const pybind11::kwargs& args);
    virtual ~Collider();
    void  shutdown() override;
    Bounds getStaticBounds() const;
    glm::vec4 bounds() const;
    std::type_index getType() final override;
    virtual std::shared_ptr<shapes::Shape> getShape() = 0;
    virtual int getCollisionTag() const;
    virtual int getCollisionFlag() const;
    virtual int getCollisionMask() const;
    void setCollisionFlag(int);
    void setCollisionMask(int);
    void setCollisionTag(int);
    void start() override;
	void update(double) override;
	using Base = Collider;
	void setResponse(int tag, const pybind11::kwargs& args);
	bool respondTo(Collider*);
	void startCollision(Collider*, glm::vec2);
	void endCollision(Collider*);
protected:
	struct ColliderResponse {
		pybind11::function onStart;
		pybind11::function onEnd;
		pybind11::function onContinue;
	};

    virtual void generateDebugMesh() {}
    int m_callbackHandle;
    Bounds m_staticBounds;
    ICollisionEngine* m_engine;
    Node* m_debugNode;
    int m_flag;
    int m_mask;
    int m_tag;


	//std::unordered_set<Collider*> _previous;
	std::unordered_set<Collider*> _current;
	std::unordered_map<int, ColliderResponse> _response;

};

inline int Collider::getCollisionTag() const {
    return m_tag;
}

inline int Collider::getCollisionFlag() const {
    return m_flag;
}

inline int Collider::getCollisionMask() const {
    return m_mask;
}


class SimpleCollider : public Collider {
public:
    SimpleCollider(int flag, int mask, int tag, std::shared_ptr<shapes::Shape> shape, const pybind11::kwargs&); //std::shared_ptr<Shape>, int flag, int mask, int tag);
    std::shared_ptr<shapes::Shape> getShape() override;
private:
    void generateDebugMesh() override;
    std::shared_ptr<shapes::Shape> m_shape;

};

inline std::shared_ptr<shapes::Shape> SimpleCollider::getShape() {
    return m_shape;
}



