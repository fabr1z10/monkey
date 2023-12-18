#include "collision_engine.h"
#include "../node.h"

CollisionEngine3D::CollisionEngine3D(float width, float height, float depth) : ICollisionEngine(width, height, depth) {
    assert(width > 0.f);
    assert(height > 0.f);
    assert(depth > 0.f);

    //m_intersector = std::make_shared<Intersector3D>();
    //m_raycast = std::make_shared<RayCaster2D>();
}


RayCastHit CollisionEngine3D::rayCastX(glm::vec3 origin, float length, int mask, Node* node) {
    RayCastHit out;
//	if (length == 0.f) {
//		return out;
//	}
    float x0 = origin.x;
    float x1 = origin.x + length;
    int istart = (x0 >= 0.0f ? 0 : -1) + static_cast<int>(x0 / m_size.x);
    int iend = (x1 >= 0.0f ? 0 : -1) + static_cast<int>(x1 / m_size.x);
    // number of cells to examine
    int cells = abs(iend - istart) + 1;
    int inc = (x1 > x0) ? 1 : -1;
    int j = (origin.y >= 0.0f ? 0 : -1) + static_cast<int>(origin.y / m_size.y);
    int k = (origin.z >= 0.0f ? 0 : -1) + static_cast<int>(origin.z / m_size.z);
    for (auto n = 0, i=istart ; n < cells; ++n, i+=inc) {
        glm::vec3 cellLocation(i, j, 0);
        auto it = m_cells.find(cellLocation);
        if (it != m_cells.end()) {
            float x_seg_start = (n == 0 ? x0 : (i + (inc == 1 ? 0 : -1)) * m_size.x);
            float x_seg_end = (n == cells - 1 ? x1 : (i + (inc == 1 ? 1 : 0)) * m_size.x);
            float x_seg_min = x_seg_start, x_seg_max = x_seg_end;
            if (inc == -1) {
                std::swap(x_seg_min, x_seg_max);
            }
            Bounds segmentBounds(glm::vec3(x_seg_min, origin.y, origin.z), glm::vec3(x_seg_max, origin.y, origin.z));
            for (const auto &c : it->second.colliders) {
                if (!c->isActive() || node == c->getNode()) {
                    continue;
                }
                int flag = c->getCollisionFlag();
                if (mask == 0 || (flag & mask) != 0) {
                    // try static bounds
                    auto shapeBounds = c->getStaticBounds();
                    if (aabbTest(segmentBounds, shapeBounds)) {
                        auto *shape = c->getShape().get();
                        if (shape->getShapeType() == ShapeType::AABB3D) {
                            float dist=0.0f;
                            if (length >= 0.0f) {
                                if (x0 < shapeBounds.min.x) {
                                    dist = shapeBounds.min.x - x0;
                                } else {
                                    dist = shapeBounds.max.x - x0;
                                }
                            } else {
                                if (x0 > shapeBounds.max.x) {
                                    dist = x0 - shapeBounds.max.x;
                                } else {
                                    dist = x0 - shapeBounds.min.x;
                                }
                            }
                            out.update(dist, c, glm::vec3(-inc, 0.f, 0.f), 0);
                        } else {
                            // otherwise do a proper check
                            auto report = m_raycast->raycastX(origin, length, shape, c->getNode()->getWorldMatrix());
                            if (report.collide) {
                                out.update(report.length, c, report.normal, report.segmentIndex);
                            }
                        }
                    }
                }
            }
            if (out.collide) {
                return out;
            }
        }
    }
}