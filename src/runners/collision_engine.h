#pragma once

#include <unordered_map>
#include <unordered_set>
#include "collision.h"
#include "../runner.h"
#include "../components/collider.h"
#include "../hashpair.h"
#include "../shapes/intersector2d.h"
#include "collision_response.h"
#include "../shapes/raycaster.h"
#include <pybind11/pybind11.h>

struct CollisionEngineCell {
    bool dirty;
    std::unordered_set<Collider*> colliders;
};

struct CollisionInfo {
    CollisionReport report;
    glm::ivec3 pos;
};



struct ColliderInfo {
    glm::ivec3 min;
    glm::ivec3 max;
    bool dirty;
};


class ICollisionEngine : public Runner {
public:
    // a 2D collision engine.
    ICollisionEngine() = default;

    virtual void add (Collider*) = 0;
    virtual void move (Collider*) = 0;
    virtual void remove (Collider*) = 0;

    std::type_index getType() override;

    // cast a ray parallel to one axis
    virtual RayCastHit rayCast(glm::vec2 origin, shapes::Direction d, float length, int mask, Node* node=nullptr) const = 0;
    virtual std::vector<ShapeCastHit> shapeCast (shapes::Shape*, const glm::mat4& transform, int mask, bool onlyFirst = false) = 0;

};

inline std::type_index ICollisionEngine::getType() {
    return std::type_index(typeid(ICollisionEngine));
}

class SpatialHashingCollisionEngine : public ICollisionEngine {
public:
    SpatialHashingCollisionEngine(float cellWidth, float cellHeight);
    void add (Collider*) override;
    void move (Collider*) override;
    void remove (Collider*) override;



    void update(double) override {}

    RayCastHit rayCast(glm::vec2 origin, shapes::Direction d, float length, int mask, Node* node=nullptr) const override;

    std::vector<ShapeCastHit> shapeCast (shapes::Shape*, const glm::mat4& transform, int mask, bool onlyFirst = false) override;
private:
    glm::ivec4 getLocation(const Bounds& b) const;
    glm::ivec4 getColliderPos(Collider* c);
    std::unordered_map<std::pair<uint32_t, uint32_t>, std::unordered_set<Collider*> > _colliders;
    std::unordered_map<Collider*, glm::ivec4> _location;
    float _width;
    float _height;
    Intersector2D _intersector;
};

