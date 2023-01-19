#pragma once

#include "../component.h"
#include "../shape.h"
#include <memory>

class CollisionEngine;

class Collider : public Component {
public:
    Collider(int flag, int mask, int tag);
    virtual ~Collider();
    Bounds getStaticBounds() const;
    glm::vec4 bounds() const;
    std::type_index getType() final override;
    virtual std::shared_ptr<Shape> getShape() = 0;
    int getCollisionTag() const;
    int getCollisionFlag() const;
    int getCollisionMask() const;
    void setCollisionFlag(int);
    void setCollisionMask(int);
    void setCollisionTag(int);
    void start() override;
protected:
    virtual void generateDebugMesh() {}
    int m_callbackHandle;
    Bounds m_staticBounds;
    CollisionEngine* m_engine;
    Node* m_debugNode;
    int m_flag;
    int m_mask;
    int m_tag;
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
    SimpleCollider(std::shared_ptr<Shape>, int flag, int mask, int tag);
    std::shared_ptr<Shape> getShape() override;
private:
    void generateDebugMesh() override;
    std::shared_ptr<Shape> m_shape;

};

inline std::shared_ptr<Shape> SimpleCollider::getShape() {
    return m_shape;
}



