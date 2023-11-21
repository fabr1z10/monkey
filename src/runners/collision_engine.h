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

class ICollisionEngine : public Runner {
public:
    ICollisionEngine(float, float, float);

    void add (Collider*);
    void move (Collider*);
    virtual void remove (Collider*) = 0;

    void update(double) override;

    void addResponse(int, int, const pybind11::kwargs&);

    // cast a ray parallel to one axis
    virtual RayCastHit rayCastX(glm::vec3 origin, float length, int mask, Node* node=nullptr) = 0;
	virtual RayCastHit rayCastY(glm::vec3 origin, float length, int mask, Node* node=nullptr) = 0;
	virtual RayCastHit rayCastZ(glm::vec3 origin, float length, int mask, Node* node=nullptr) = 0;

	virtual std::pair<glm::ivec3, glm::ivec3> getLocation(const Bounds& b) = 0;
	const CollisionEngineCell* getColliders(glm::ivec3);

protected:
    virtual bool aabbTest(const Bounds& b1, const Bounds& b2) = 0;
	int getIndex(float x, Direction d);
    virtual void pushCollider(Collider* c, glm::ivec3 m, glm::ivec3 M) = 0;
    std::unordered_map<Collider*, ColliderInfo> m_colliderLocations;
    std::unordered_map<glm::ivec3, CollisionEngineCell> m_cells;
    std::unordered_set<Collider*> m_removed;
    // TODO add response manager --> object that holds the callback for collision
    std::shared_ptr<CollisionResponseManager> m_responseManager;
    std::shared_ptr<Intersector> m_intersector;
    std::shared_ptr<RayCaster> m_raycast;
    std::unordered_map<std::pair<Collider*, Collider*>, CollisionInfo> m_previouslyCollidingPairs;

    bool _staticTest;

	glm::vec3 m_size;
};

inline int ICollisionEngine::getIndex(float u, Direction d) {
	return m_size[d] == 0.f ? 0 : (-1 * (u < 0) + static_cast<int>(u / m_size[d]));
}

class CollisionEngine2D : public ICollisionEngine {
public:
    CollisionEngine2D(float, float);

	RayCastHit rayCastX(glm::vec3 origin, float length, int mask, Node* node=nullptr) override;
	RayCastHit rayCastY(glm::vec3 origin, float length, int mask, Node* node=nullptr) override;
	RayCastHit rayCastZ(glm::vec3 origin, float length, int mask, Node* node=nullptr) override;
	std::type_index getType() override;
	void remove (Collider*) override;
	std::pair<glm::ivec3, glm::ivec3> getLocation(const Bounds& b) override;

protected:
	virtual bool aabbTest(const Bounds& b1, const Bounds& b2) override;

	void pushCollider(Collider* c, glm::ivec3 m, glm::ivec3 M) override;



};


//class CollisionEngine3D : public ICollisionEngine {
//public:
//    CollisionEngine3D(float, float, float);
//    //void add (Collider*);
//    void remove (Collider*) override;
//
//    void update(double) override;
//    RayCastHit rayCast(glm::vec3 rayOrigin, glm::vec3 rayDir, float length, int mask);
//    virtual std::vector<ShapeCastHit> shapeCast (Shape*, const glm::mat4& transform, int mask, bool onlyFirst = false) ;
//
//    const CollisionEngineCell* getColliders(glm::ivec3);
//    std::pair<glm::ivec3, glm::ivec3> getLocation(const Bounds& b);
//    void processCollisions(const std::vector<ShapeCastHit>&, Node*, int);
//
//protected:
//
//    void pushCollider(Collider* c, glm::ivec3 m, glm::ivec3 M);
//    int getIndex(float, float);
//    glm::vec3 m_size;
//
//    // TODO add intersector --> that is the object that does the actual shape intersection
//
//
//
//
//    bool m_2d;
//};
