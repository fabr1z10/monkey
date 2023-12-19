#include "collision_engine.h"
#include "../node.h"

CollisionEngine3D::CollisionEngine3D(float width, float height, float depth) : ICollisionEngine(width, height, depth) {
    assert(width > 0.f);
    assert(height > 0.f);
    assert(depth > 0.f);
    _use3D = true;

    //m_intersector = std::make_shared<Intersector3D>();
    //m_raycast = std::make_shared<RayCaster2D>();
}

std::type_index CollisionEngine3D::getType() {
    return std::type_index(typeid(ICollisionEngine));
}

bool CollisionEngine3D::aabbTest(const Bounds &b1, const Bounds &b2) {

    return !(b1.min.x > b2.max.x || b1.max.x < b2.min.x ||
             b1.min.y > b2.max.y || b1.max.y < b2.min.y ||
             b1.min.z > b2.max.z || b1.max.z < b2.min.z);


}

void CollisionEngine3D::pushCollider(Collider* c, glm::ivec3 m, glm::ivec3 M) {
    auto it = m_colliderLocations.find(c);
    if (it != m_colliderLocations.end()) {
        if (it->second.min != m || it->second.max != M) {
            for (int i = it->second.min.x; i <= it->second.max.x; ++i) {
                for (int j = it->second.min.y; j <= it->second.max.y; ++j) {
                    for (int k = it->second.min.z; k <= it->second.max.z; ++k) {
                        m_cells[glm::ivec3(i, j, k)].colliders.erase(c);
                    }
                }
            }
        }
    }
    for (int i = m.x; i <= M.x; ++i) {
        for (int j = m.y; j <= M.y; ++j) {
            for (int k = m.z; k <= M.z; ++k) {
                auto &cell = m_cells[glm::ivec3(i, j, k)];
                cell.colliders.insert(c);
                cell.dirty = true;
            }
        }
    }
    m_colliderLocations[c] = ColliderInfo {m, M, true};
}


