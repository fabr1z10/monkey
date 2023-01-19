#pragma once

#include <unordered_map>
#include <unordered_set>
#include "collision.h"
#include "../runner.h"
#include "../components/collider.h"
#include "../hashpair.h"
#include "../shapes/intersector.h"
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

class CollisionEngine : public Runner {
public:
    CollisionEngine(float, float, float);
    void add (Collider*);
    void remove (Collider*);
    void move (Collider*) ;
    void update(double) override;
    RayCastHit rayCast(glm::vec3 rayOrigin, glm::vec3 rayDir, float length, int mask);
    virtual std::vector<ShapeCastHit> shapeCast (Shape*, const glm::mat4& transform, int mask, bool onlyFirst = false) ;

    void addResponse(int, int, const pybind11::kwargs&);
    const CollisionEngineCell* getColliders(glm::ivec3);
    std::pair<glm::ivec3, glm::ivec3> getLocation(const Bounds& b);
    void processCollisions(const std::vector<ShapeCastHit>&, Node*, int);

protected:
    bool aabbTest(const Bounds& b1, const Bounds& b2);
    std::unordered_map<glm::ivec3, CollisionEngineCell> m_cells;
    std::unordered_map<Collider*, ColliderInfo> m_colliderLocations;
    void pushCollider(Collider* c, glm::ivec3 m, glm::ivec3 M);
    int getIndex(float, float);
    glm::vec3 m_size;

    // TODO add intersector --> that is the object that does the actual shape intersection
    std::shared_ptr<Intersector> m_intersector;

    std::shared_ptr<RayCaster> m_raycast;

    // TODO add response manager --> object that holds the callback for collision
    std::shared_ptr<CollisionResponseManager> m_responseManager;

    std::unordered_map<std::pair<Collider*, Collider*>, CollisionInfo> m_previouslyCollidingPairs;
    std::unordered_set<Collider*> m_removed;
    bool m_2d;
};
