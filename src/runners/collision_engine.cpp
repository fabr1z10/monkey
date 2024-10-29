#include <iostream>
#include "collision_engine.h"
#include "../node.h"
#include "../upair.h"
#include "../shapes/convexpoly.h"
#include "../shapes/intersector2d.h"
//#include "../shapes/intersector3d.h"
//#include "../shapes3d/seg3d.h"
using namespace shapes;

//std::vector<ShapeCastHit> ICollisionEngine::shapeCast(Shape * shape, const glm::mat4 &transform, int mask, bool onlyFirst) {
////    std::vector<ShapeCastHit> result;
////
//    // first get the transformed aabb
//    auto aabb = shape->getBounds();
//    aabb.transform(transform);
//    std::cout << aabb.getSize().x << ", " << aabb.getSize().y << "\n";
//
////    auto loc = getLocation(aabb);
////
////    for (int i = loc.first.x; i <= loc.second.x; ++i) {
////        for (int j = loc.first.y; j <= loc.second.y; ++j) {
////            for (int k = loc.first.z; k <= loc.second.z; ++k) {
////                auto cell = m_cells.find(glm::vec3(i, j, k));
////                if (cell != m_cells.end()) {
////                    auto &colliders = cell->second.colliders;
////                    for (auto &c : colliders) {
////                        if (c->getState() != NodeState::ACTIVE) {
////                            continue;
////                        }
////                        int flag = c->getCollisionFlag();
////                        int m = flag & mask;
////                        if (m == 0) {
////                            continue;
////                        }
////                        auto b = c->getStaticBounds();
////                        // perform a aabb testing
////                        if (!aabbTest(aabb, b)) {
////                            continue;
////                        }
////                        auto s = c->getShape();
////                        if (s != nullptr) {
////                            CollisionReport report;
////                            if (shape->getShapeType() == ShapeType::AABB3D && s->getShapeType() == ShapeType::AABB3D) {
////                                // special case, skip test
////                                report.collide = true;
////                            } else {
////                                const auto &t = c->getNode()->getWorldMatrix();
////                                report = m_intersector->intersect(shape, s.get(), transform, t);
////                            }
////                            if (report.collide) {
////                                Bounds bb = aabb.intersection(b);
////                                ShapeCastHit res;
////                                res.report = report;
////                                res.report.direction = bb.getCenter();
////                                res.entity = c;
////                                result.push_back(res);
////                                if (onlyFirst)
////                                    return result;
////                            }
////                        }
////                    }
////                }
////            }
////        }
////    }
////    return result;
//}

RayCastHit SpatialHashingCollisionEngine::rayCast(glm::vec2 origin, Direction d, float length, int mask, Node *node) const {

    RayCastHit out;


    // index of origin cell
    int ix0 = static_cast<int>(origin.x / _width);
    int iy0 = static_cast<int>(origin.y / _height);

    glm::vec2 target{origin};
    int ix1{ix0};
    int iy1{iy0};
    int cellsToCheck {0};
    int incx = 0;
    int incy = 0;
    Bounds b;
    if ( d == Direction::X) {

        target.x += length;
        ix1 = static_cast<int>(target.x / _width);
        cellsToCheck = fabs(ix1 - ix0) + 1;
        b.min = glm::vec3(length > 0 ? origin.x : target.x, origin.y, 0.f);
        b.max = glm::vec3(length > 0 ? target.x : origin.x, origin.y, 0.f);
        incx = length > 0 ? 1 : -1;
    } else {
        target.y += length;
        iy1 = static_cast<int>(target.x / _width);
        cellsToCheck = fabs(iy1 - iy0) + 1;
        b.min = glm::vec3(origin.x, length > 0 ? origin.y : target.y, 0.f);
        b.max = glm::vec3(origin.x, length > 0 ? target.y : origin.y, 0.f);
        incy = length > 0 ? 1 : -1;
    }

    std::pair<int, int> current(ix0, iy0);

    for (int i = 0; i < cellsToCheck; ++i) {
        // get the colliders at this cell
        auto it = _colliders.find(current);
        if (it == _colliders.end()) {
            continue;
        }
        // now loop colliders
        for (const auto& c : it->second) {
            // check mask
            auto flag = c->getCollisionFlag();
            if (c->getNode() == node) continue;
            if (mask == 0 || (flag & mask) != 0) {

                //c->getShape();
                auto cb = c->getStaticBounds();

                if (cb.intersect2D(b)) {
                    // we need to transform the ray into local coordinates
                    auto mat = glm::inverse(c->getNode()->getWorldMatrix());
                    auto lP0 = mat * glm::vec4(origin, 0.f, 1.f);
                    auto lP1 = mat * glm::vec4(target, 0.f, 1.f);

                    auto result = c->getShape()->raycast(lP0, lP1);
                    if (result.collide) {
                        out.collide = true;
                        out.entity = c;
                        out.normal = result.normal;
                        out.length = result.length;

                        return out;
                    }
                }
                //c->getNode()->getWorldMatrix();

                // transform world points in local coordinates, in order to do a raycast in local
            }
            // check itself
        }
        current.first += incx;
		current.second += incy;

    }
    return out;
//
//    int ic = i0;
//    int jc = j0;
//    glm::vec2 u = glm::normalize(target - source);
//    glm::vec2 oc(source);
//    ShapeType boxType = _use3D ? ShapeType::AABB3D : ShapeType::AABB2D;
//
//    do {
//        glm::vec2 tc ;
//        int inc_i {0};
//        int inc_j {0};
//        if (ic == i1 && jc == j1) {
//            tc = glm::vec2(target);
//        } else {
//            // consider only the subsegment in cell (ic, jc)
//            float ny = u.y >= 0 ? (jc + 1) * m_size.y : (jc * m_size.y);
//            float nx = u.x >= 0 ? (ic + 1) * m_size.y : (ic * m_size.y);
//            float dist_x = u.x >= 0 ? nx - oc.x : oc.x - nx;
//            float dist_y = u.y >= 0 ? ny - oc.y : oc.y - ny;
//            if (u.x == 0.f) {
//                tc = glm::vec2(oc.x, ny);
//                inc_j += u.y > 0 ? 1 : -1;
//            } else if (u.y == 0.f) {
//                tc = glm::vec2(nx, oc.y);
//                inc_i += u.x > 0 ? 1 : -1;
//            } else {
//                glm::vec2 a(dist_x / fabs(u.x) , dist_y / fabs(u.y));
//                if (a.x < a.y) {
//                    tc = glm::vec2(nx, oc.y + a.x * u.y);
//                    inc_i += u.x > 0 ? 1 : -1;
//                } else {
//                    tc = glm::vec2(oc.x + a.y * u.x, ny);
//                    inc_j += u.y > 0 ? 1 : -1;
//                }
//            }
//        }
//
//
//        // collision check on (ic, jc)
//
//        auto it = m_cells.find(glm::ivec3(ic, jc, 0));
//        if (it != m_cells.end()) {
//            Bounds segmentBounds(glm::vec3(oc.x, oc.y, 0.f), glm::vec3(tc.x, tc.y, 0.f));
//            for (const auto &c : it->second.colliders) {
//                if (c->getState() != NodeState::ACTIVE || self == c->getNode()) {
//                    continue;
//                }
//                int flag = c->getCollisionFlag();
//                if (mask == 0 || (flag & mask) != 0) {
//                    auto shapeBounds = c->getStaticBounds();
//                    if (aabbTest(segmentBounds, shapeBounds)) {
//                        auto *shape = c->getShape().get();
//                        if (shape->getShapeType() == boxType) {
//                            out.update(0.f, c, glm::vec3(0.f), 0);
//                        } else {
//                            // otherwise do a proper check
//                            auto report = m_raycast->raycast(glm::vec3(oc, 0.f), glm::vec3(tc, 0.f), shape,
//                                                             c->getNode()->getWorldMatrix());
//                            if (report.collide) {
//                                out.update(report.length, c, report.normal, report.segmentIndex);
//                            }
//                        }
//                    }
//                }
//            }
//        }
//        oc = tc;
//        if (ic == i1 && jc == j1) {
//            break;
//        }
//        ic += inc_i;
//        jc += inc_j;
//
//
//    } while (true);
//
//    return out;

}

//
//RayCastHit ICollisionEngine::rayCastX(glm::vec3 origin, float length, int mask, Node* node) {
//    RayCastHit out;
//    float x0 = origin.x;
//    float x1 = origin.x + length;
//    int istart = (x0 >= 0.0f ? 0 : -1) + static_cast<int>(x0 / m_size.x);
//    int iend = (x1 >= 0.0f ? 0 : -1) + static_cast<int>(x1 / m_size.x);
//    // number of cells to examine
//    int cells = abs(iend - istart) + 1;
//    int inc = (x1 > x0) ? 1 : -1;
//    int j = (origin.y >= 0.0f ? 0 : -1) + static_cast<int>(origin.y / m_size.y);
//    int k = _use3D ? ((origin.z >= 0.0f ? 0 : -1) + static_cast<int>(origin.z / m_size.z)) : 0;
//    float z = _use3D ? origin.z : 0.f;
//    ShapeType boxType = _use3D ? ShapeType::AABB3D : ShapeType::AABB2D;
//    for (auto n = 0, i=istart ; n < cells; ++n, i+=inc) {
//        glm::vec3 cellLocation(i, j, k);
//        auto it = m_cells.find(cellLocation);
//        if (it != m_cells.end()) {
//            float x_seg_start = (n == 0 ? x0 : (i + (inc == 1 ? 0 : -1)) * m_size.x);
//            float x_seg_end = (n == cells - 1 ? x1 : (i + (inc == 1 ? 1 : 0)) * m_size.x);
//            float x_seg_min = x_seg_start, x_seg_max = x_seg_end;
//            if (inc == -1) {
//                std::swap(x_seg_min, x_seg_max);
//            }
//            Bounds segmentBounds(glm::vec3(x_seg_min, origin.y, z), glm::vec3(x_seg_max, origin.y, z));
//            for (const auto &c : it->second.colliders) {
//                if (c->getState() != NodeState::ACTIVE || node == c->getNode()) {
//                    continue;
//                }
//                int flag = c->getCollisionFlag();
//                if (mask == 0 || (flag & mask) != 0) {
//                    // try static bounds
//                    auto shapeBounds = c->getStaticBounds();
//                    if (aabbTest(segmentBounds, shapeBounds)) {
//                        auto *shape = c->getShape().get();
//                        if (shape->getShapeType() == boxType) {
//                            float dist=0.0f;
//                            if (length >= 0.0f) {
//                                if (x0 < shapeBounds.min.x) {
//                                    dist = shapeBounds.min.x - x0;
//                                } else {
//                                    dist = shapeBounds.max.x - x0;
//                                }
//                            } else {
//                                if (x0 > shapeBounds.max.x) {
//                                    dist = x0 - shapeBounds.max.x;
//                                } else {
//                                    dist = x0 - shapeBounds.min.x;
//                                }
//                            }
//                            if (dist > fabs(length)) return out;
//                            out.update(dist, c, glm::vec3(-inc, 0.f, 0.f), 0);
//                        } else {
//                            // otherwise do a proper check
//                            auto report = m_raycast->raycastX(origin, length, shape, c->getNode()->getWorldMatrix());
//                            if (report.collide) {
//                                out.update(report.length, c, report.normal, report.segmentIndex);
//                            }
//                        }
//                    }
//                }
//            }
//            if (out.collide) {
//                return out;
//            }
//        }
//    }
//}
//
//RayCastHit ICollisionEngine::rayCastY(glm::vec3 origin, float length, int mask, Node* node) {
//    RayCastHit out;
////	if (length == 0.f) {
////		return out;
////	}
//    float y0 = origin.y;
//    float y1 = origin.y + length;
//    int istart = (y0 >= 0.0f ? 0 : -1) + static_cast<int>(y0 / m_size.y);
//    int iend = (y1 >= 0.0f ? 0 : -1) + static_cast<int>(y1 / m_size.y);
//    // number of cells to examine
//    int cells = abs(iend - istart) + 1;
//    int inc = (y1 > y0) ? 1 : -1;
//    int j = (origin.x >= 0.0f ? 0 : -1) + static_cast<int>(origin.x / m_size.x);
//    int k = _use3D ? ((origin.z >= 0.0f ? 0 : -1) + static_cast<int>(origin.z / m_size.z)) : 0;
//    float z = _use3D ? origin.z : 0.f;
//    ShapeType boxType = _use3D ? ShapeType::AABB3D : ShapeType::AABB2D;
//    for (auto n = 0, i=istart ; n < cells; ++n, i+=inc) {
//        glm::vec3 cellLocation(j, i, k);
//        auto it = m_cells.find(cellLocation);
//        if (it != m_cells.end()) {
//            float y_seg_start = (n == 0 ? y0 : (i + (inc == 1 ? 0 : -1)) * m_size.y);
//            float y_seg_end = (n == cells - 1 ? y1 : (i + (inc == 1 ? 1 : 0)) * m_size.y);
//            float y_seg_min = y_seg_start, y_seg_max = y_seg_end;
//            if (inc == -1) {
//                std::swap(y_seg_min, y_seg_max);
//            }
//            Bounds segmentBounds(glm::vec3(origin.x, y_seg_min, z), glm::vec3(origin.x, y_seg_max, z));
//            for (const auto &c : it->second.colliders) {
//                if (c->getState() != NodeState::ACTIVE || node == c->getNode()) {
//                    continue;
//                }
//                int flag = c->getCollisionFlag();
//                if (mask == 0 || (flag & mask) != 0) {
//                    // try static bounds
//                    auto shapeBounds = c->getStaticBounds();
//                    if (aabbTest(segmentBounds, shapeBounds)) {
//                        auto *shape = c->getShape().get();
//                        if (shape->getShapeType() == boxType) {
//                            float dist=0.0f;
//                            if (length >= 0.0f) {
//                                if (y0 < shapeBounds.min.y) {
//                                    dist = shapeBounds.min.y - y0;
//                                } else {
//                                    dist = shapeBounds.max.y - y0;
//                                }
//                            } else {
//                                if (y0 > shapeBounds.max.y) {
//                                    dist = y0 - shapeBounds.max.y;
//                                } else {
//                                    dist = y0 - shapeBounds.min.y;
//                                }
//                            }
//                            if (dist > fabs(length)) return out;
//                            out.update(dist, c, glm::vec3(0.0f, -inc, 0.f), 0);
//                        } else {
//                            // otherwise do a proper check
//                            auto report = m_raycast->raycastY(origin, length, shape, c->getNode()->getWorldMatrix());
//                            if (report.collide) {
//                                out.update(report.length, c, report.normal, report.segmentIndex);
//                            }
//                        }
//                    }
//                }
//            }
//            if (out.collide) {
//                return out;
//            }
//        }
//    }
//}
//
//RayCastHit ICollisionEngine::rayCastZ(glm::vec3 origin, float length, int mask, Node* node) {
//    RayCastHit out;
//    if (!_use3D) {
//        return out;
//    }
//    float z0 = origin.z;
//    float z1 = origin.z + length;
//    int kstart = (z0 >= 0.0f ? 0 : -1) + static_cast<int>(z0 / m_size.z);
//    int kend = (z1 >= 0.0f ? 0 : -1) + static_cast<int>(z1 / m_size.z);
//    // number of cells to examine
//    int cells = abs(kend - kstart) + 1;
//    int inc = (z1 > z0) ? 1 : -1;
//    int i = (origin.x >= 0.0f ? 0 : -1) + static_cast<int>(origin.x / m_size.x);
//    int j = (origin.y >= 0.0f ? 0 : -1) + static_cast<int>(origin.y / m_size.y);
//    //int k = _use3D ? ((origin.z >= 0.0f ? 0 : -1) + static_cast<int>(origin.z / m_size.z)) : 0;
//    //float z = _use3D ? origin.z : 0.f;
//    //ShapeType boxType = _use3D ? ShapeType::AABB3D : ShapeType::AABB2D;
//    for (auto n = 0, k=kstart ; n < cells; ++n, k+=inc) {
//        glm::vec3 cellLocation(i, j, k);
//        auto it = m_cells.find(cellLocation);
//        if (it != m_cells.end()) {
//            float z_seg_start = (n == 0 ? z0 : (i + (inc == 1 ? 0 : -1)) * m_size.z);
//            float z_seg_end = (n == cells - 1 ? z1 : (i + (inc == 1 ? 1 : 0)) * m_size.z);
//            float z_seg_min = z_seg_start, z_seg_max = z_seg_end;
//            if (inc == -1) {
//                std::swap(z_seg_min, z_seg_max);
//            }
//            Bounds segmentBounds(glm::vec3(origin.x, origin.y, z_seg_min), glm::vec3(origin.x, origin.y, z_seg_max));
//            for (const auto &c : it->second.colliders) {
//                if (c->getState() != NodeState::ACTIVE || node == c->getNode()) {
//                    continue;
//                }
//                int flag = c->getCollisionFlag();
//                if (mask == 0 || (flag & mask) != 0) {
//                    // try static bounds
//                    auto shapeBounds = c->getStaticBounds();
//                    if (aabbTest(segmentBounds, shapeBounds)) {
//                        auto *shape = c->getShape().get();
//                        if (shape->getShapeType() == ShapeType::AABB3D) {
//                            float dist=0.0f;
//                            if (length >= 0.0f) {
//                                if (z0 < shapeBounds.min.z) {
//                                    dist = shapeBounds.min.z - z0;
//                                } else {
//                                    dist = shapeBounds.max.z - z0;
//                                }
//                            } else {
//                                if (z0 > shapeBounds.max.z) {
//                                    dist = z0 - shapeBounds.max.z;
//                                } else {
//                                    dist = z0 - shapeBounds.min.z;
//                                }
//                            }
//                            out.update(dist, c, glm::vec3(0.0f, 0.0f, -inc), 0);
//                        } else {
//                            // otherwise do a proper check
//                            auto report = m_raycast->raycastZ(origin, length, shape, c->getNode()->getWorldMatrix());
//                            if (report.collide) {
//                                out.update(report.length, c, report.normal, report.segmentIndex);
//                            }
//                        }
//                    }
//                }
//            }
//            if (out.collide) {
//                return out;
//            }
//        }
//    }
//
//}

void SpatialHashingCollisionEngine::move(Collider * c) {
    // check if collider is already in
    auto it = _location.find(c);
    if (it == _location.end()) {
        add(c);
    } else {
        auto pos = getColliderPos(c);
        if (pos != it->second) {
            remove(c);
            add(c);
        }
    }
}

glm::ivec4 SpatialHashingCollisionEngine::getLocation(const Bounds &b) const {
    glm::ivec4 p;
    p[0] = static_cast<uint32_t>(b.min.x / _width);
    p[1] = static_cast<uint32_t>(b.max.x / _width);
    p[2] = static_cast<uint32_t>(b.min.y / _height);
    p[3] = static_cast<uint32_t>(b.max.y / _height);
    return p;

}

glm::ivec4 SpatialHashingCollisionEngine::getColliderPos(Collider *c) {
    auto aabb = c->getStaticBounds();
    return getLocation(aabb);
}

void SpatialHashingCollisionEngine::add(Collider * c) {
    // this is called when a new collider starts. It registers with the engine
    // get the shape bounding box, transform it, map it
    auto pos = getColliderPos(c);
    for (uint32_t i = pos[0]; i <= pos[1]; ++i) {
        for (uint32_t j = pos[2]; j <= pos[3]; ++j) {
            _colliders[std::make_pair(i, j)].insert(c);
        }
    }
    _location[c] = pos;
}

void SpatialHashingCollisionEngine::remove(Collider * c) {
    auto it = _location.find(c);
    if (it != _location.end()) {
        glm::ivec4 pos = it->second;
        for (uint32_t i = pos[0]; i <= pos[1]; ++i) {
            for (uint32_t j = pos[2]; j <= pos[3]; ++j) {
                _colliders[std::make_pair(i, j)].insert(c);
            }
        }
    }
    _location.erase(c);
}

SpatialHashingCollisionEngine::SpatialHashingCollisionEngine(float width, float height) : ICollisionEngine(),
    _width(width), _height(height)
{

}


//
//std::type_index CollisionEngine2D::getType() {
//	return std::type_index(typeid(ICollisionEngine));
//}
//
//
//CollisionEngine2D::CollisionEngine2D(float width, float height) : ICollisionEngine(width, height, 0.f) {
//	assert(width > 0.f);
//	assert(height > 0.f);
//    _use3D = false;
//    m_intersector = std::make_shared<Intersector2D>();
//	m_raycast = std::make_shared<RayCaster2D>();
//}

// to specify a 2D engine, just set depth to 0
//CollisionEngine3D::CollisionEngine3D(float width, float height, float depth) : ICollisionEngine(width, height, depth) {
//	assert(width > 0.f);
//	assert(height > 0.f);
//	assert(depth >= 0.f);
//
//    //m_intersector = std::make_shared<Intersector3D>();
//    //m_raycast = std::make_shared<RayCaster3D>();
//
//}






//
//void ICollisionEngine::remove(Collider * c) {
//    m_removed.insert(c);
//    if (m_colliderLocations.find(c) != m_colliderLocations.end()) {
//		auto d = m_colliderLocations.at(c);
//		for (auto i = d.min.x; i <= d.max.x; ++i) {
//			for (auto j = d.min.y; j <= d.max.y; ++j) {
//				for (auto k = d.min.z; k <= d.max.z; ++k) {
//					auto aa = m_cells.find(glm::vec3(i, j, k));
//					if (aa != m_cells.end()) {
//						aa->second.colliders.erase(c);
//					}
//				}
//			}
//		}
//		m_colliderLocations.erase(c);
//	}
////    for (auto it = m_previouslyCollidingPairs.begin(); it != m_previouslyCollidingPairs.end(); ) {
////        if (it->first.first == c || it->first.second == c) {
////            it = m_previouslyCollidingPairs.erase(it);
////        } else {
////            it++;
////        }
////    }
//}

//void ICollisionEngine::move(Collider * c) {
//    // TODO mark collider as DIRTY, and update collider position (cells occupied)
//    // then at update time, go thorugh each dirty collider, and test it with other colliders in cells
//    // to avoid double testing, keep track of tested pairs
//    auto aabb = c->getStaticBounds();
//    auto loc = getLocation(aabb);
//    pushCollider(c, loc.first, loc.second);			// this automatically sets it to dirty
//    //std::cout << "aabb: (" << aabb.min.x << ", " << aabb.min.y << "), (" << aabb.max.x << ", " << aabb.max.y << ")\n";
//}
//
//void ICollisionEngine::update(double) {
//
//    std::unordered_map<std::pair<Collider*, Collider*>, CollisionInfo> currentlyCollidingPairs;
//    std::unordered_set<glm::ivec3> cellsExamined;
//
//    unsigned collisionChecks = 0;
//    int cc = 0;
//    std::unordered_set<UPair<Collider*>, UPair<Collider*>::HashFunction> processed;
//    for (auto& c : m_colliderLocations) {
//        if (c.second.dirty) {
//            auto *c1 = c.first;
//            if (c1->getState() != NodeState::ACTIVE) {
//            	continue;
//            }
//            // loop through cells occupied by collider
//            for (auto i = c.second.min.x; i <= c.second.max.x; ++i) {
//                for (auto j = c.second.min.y; j <= c.second.max.y; ++j) {
//                    for (auto k = c.second.min.z; k <= c.second.max.z; ++k) {
//                        // doing cell (i, j, k)
//                        const auto &colliders = m_cells.at(glm::ivec3(i, j, k)).colliders;
//                        if (colliders.size() <= 1) {
//                            continue;
//                        }
//                        for (const auto &c2 : colliders) {
//                            // don't collide with itself
//                            auto pair = UPair<Collider *>(c1, c2);
//                            if (processed.find(pair) != processed.end()) continue;
//                            processed.insert(UPair<Collider *>(c1, c2));
//                            if (c1 == c2 || c2->getState() != NodeState::ACTIVE) continue;
//                            if (((c1->getCollisionMask() & c2->getCollisionFlag()) == 0) ||
//                                ((c2->getCollisionMask() & c1->getCollisionFlag()) == 0)) {
//                                continue;
//                            }
//                            // if no response is provided for these tags, then skip it
//                            if (!c1->respondTo(c2) && !c2->respondTo(c1)) {
//                                continue;
//                            }
//                            // we have a collision response, so let's calculate collision
//                            collisionChecks++;
//                            auto b1 = c1->getStaticBounds();
//                            auto b2 = c2->getStaticBounds();
//                            CollisionReport report;
//                            if (aabbTest(b1, b2)) {
//	                            // get the shape in local coordinates
//    	                        const auto &t1 = c1->getNode()->getWorldMatrix();
//        	                    const auto &t2 = c2->getNode()->getWorldMatrix();
//
//            	                report = m_intersector->intersect(c1->getShape().get(), c2->getShape().get(), t1, t2);
//
//                                //rrentlyCollidingPairs.insert(std::make_pair(std::make_pair(c1, c2), ci));
//                            }
//							if (report.collide) {
//								//CollisionInfo ci;
//								//ci.report = report;
//								//ci.pos = glm::vec3(i, j, k);
//								c1->startCollision(c2);
//								c2->startCollision(c1);
//
//							} else {
//								c1->endCollision(c2);
//								c2->endCollision(c1);
//							}
//
//						}
//                    }
//                }
//            }
//        }
//
//        c.second.dirty = false;
//
//    }
////    if (cc> 0) {
////		std::cout << "---" << cc << "\n";
////	}
//
//    // remove pairs that were previously colliding but not now
////    for (auto iter = m_previouslyCollidingPairs.begin(); iter != m_previouslyCollidingPairs.end();) {
////        CollisionInfo& ci = iter->second;
////        // If i have examined the cell AND they are not colliding anymore ...
////        if (processed.count(UPair(iter->first.first, iter->first.second)) > 0 && currentlyCollidingPairs.count(iter->first) == 0) {
////            m_responseManager->onEnd (iter->first.first, iter->first.second);
////            m_previouslyCollidingPairs.erase(iter++);
////        }
////        else {
////            iter++;
////        }
////    }
////
////    for (auto& p : currentlyCollidingPairs) {
////        auto it = m_previouslyCollidingPairs.find(p.first);
////        if (it == m_previouslyCollidingPairs.end()) {
////            m_responseManager->onStart (p.first.first, p.first.second, p.second.report.direction * p.second.report.distance);
////            m_previouslyCollidingPairs.insert(std::make_pair(p.first, p.second));
////        } else {
////            m_responseManager->onStay(p.first.first, p.first.second);
////            it->second = p.second;
////        }
////    }
//}
//
//void CollisionEngine2D::pushCollider(Collider* c, glm::ivec3 m, glm::ivec3 M) {
//    auto it = m_colliderLocations.find(c);
//    if (it != m_colliderLocations.end()) {
//        if (it->second.min != m || it->second.max != M) {
//            for (int i = it->second.min.x; i <= it->second.max.x; ++i) {
//                for (int j = it->second.min.y; j <= it->second.max.y; ++j) {
//                    m_cells[glm::ivec3(i, j, 0)].colliders.erase(c);
//                }
//            }
//        }
//    }
//    for (int i = m.x; i <= M.x; ++i) {
//        for (int j = m.y; j <= M.y; ++j) {
//            auto &cell = m_cells[glm::ivec3(i, j, 0)];
//            cell.colliders.insert(c);
//            cell.dirty = true;
//        }
//    }
//    m_colliderLocations[c] = ColliderInfo {m, M, true};
//}
//
//
//
//std::pair<glm::ivec3, glm::ivec3> ICollisionEngine::getLocation(const Bounds &b) {
//
//    b.min.x / _c
//
//    glm::ivec3 min(0);
//    glm::ivec3 max(0);
//    min.x = getIndex(b.min.x, Direction::X);
//    min.y = getIndex(b.min.y, Direction::Y);
//    min.z = getIndex(b.min.z, Direction::Z);
//    max.x = getIndex(b.max.x, Direction::X);
//    max.y = getIndex(b.max.y, Direction::Y);
//    max.z = getIndex(b.max.z, Direction::Z);
//    return std::make_pair(min, max);
//}
//


//void ICollisionEngine::addResponse(int i, int j, const pybind11::kwargs& args) {
//    m_responseManager->add(i, j, args);
//}

//RayCastHit CollisionEngine::rayCast(glm::vec3 rayOrigin, glm::vec3 rayDir, float length, int mask) {
//    if (m_2d) {
//        rayOrigin.z = 0.f;
//        rayDir.z = 0.f;
//    }
//    glm::vec3 P = rayOrigin;
//    glm::vec3 P1 = P;
//    glm::vec3 P2 = P;
//    //float z = rayOrigin.z;
//
//    // initialize current cell
//    int i = getIndex(P.x, m_size[0]);
//    int j = getIndex(P.y, m_size[1]);
//    // for 2D coll engine --> k must always be 0 ?
//    int k = getIndex(P.z, m_size[2]);			// for 2D m_size[2] == 0 --> k = 0
//
//    int n = (rayDir.x > 0 ? 1 : 0);
//    int m = (rayDir.y > 0 ? 1 : 0);
//    int q = (rayDir.z > 0 ? 1 : 0);
//
//    // length traversed so far
//    float l = 0.0f;
//    bool endReached = false;
//
//    // increments
//    int id = 0, jd = 0, kd = 0;
//    RayCastHit out;
//    out.length = length;
//
//    // we can (and we MUST) exit the loop as soon as we find a collision
//    glm::ivec3 increments(rayDir.x > 0 ? 1 : -1, rayDir.y > 0 ? 1 : -1, rayDir.z > 0 ? 1 : -1);
//    while (!endReached && !out.collide) {
//
//        // gets the unit increments along the different axis to hit the cell boundary
//        // tx , ty and tz should always be positive: if say raidir.x < 0 also the numerator should be neg
//        // this allow us to see what is the next cell to examine
//        id = increments.x;
//        jd = 0;
//        kd = 0;
//        float tx = (rayDir.x == 0.0f) ? std::numeric_limits<float>::infinity() : ((i+n) * m_size.x - P.x) / rayDir.x;
//        float tbest = tx;
//        float ty = (rayDir.y == 0.0f) ? std::numeric_limits<float>::infinity() : ((j+m) * m_size.y - P.y) / rayDir.y;
//        if (ty <= tbest) {
//            id = 0;
//            jd = increments.y;
//            tbest = ty;
//        }
//        // for 2D tz must always be inf
//        float tz = (rayDir.z == 0.0f) ? std::numeric_limits<float>::infinity() : ((k+q) * m_size.z - P.z) / rayDir.z;
//        if (tz <= tbest) {
//            id = 0;
//            jd = 0;
//            kd = increments.z;
//            tbest = tz;
//        }
//
//        float tm {0.0f};
//        id = 0;
//        jd = 0;
//        kd = 0;
//        if (tx <= ty) {
//            tm = tx;
//            id = rayDir.x > 0 ? 1 : -1;
//        } else {
//            tm = ty;
//            jd = rayDir.y > 0 ? 1 : -1;
//        }
//
//        // advance by tm TODO are u sure?
//        if (l + tm < length) {
//            // need to add a tiny extra bit in case the colliding object is a line that lies exactly at the border
//            // of two neighboring cell!
//            P1 = P + (tm + 0.01f) * rayDir;
//            P2 = P + (tm - 0.01f) * rayDir;
//            // add tm to the cumulated length done
//            l += tm;
//        } else {
//            P1 = P + (length - l) * rayDir;
//            endReached = true;
//        }
//
//        // get the colliders at the current cell
//        auto it = m_cells.find(glm::ivec3(i, j, k));
//        Segment3D line(P, P1);
//        auto lineBounds = line.getBounds();
//        if (it != m_cells.end()) {
//            for (auto& c : it->second.colliders) {
//                if (!c->isActive())
//                    continue;
//                // aabb check
//                int flag = c->getCollisionFlag();
//                int fm = flag & mask;
//                if (fm != 0) {
//                    auto shapeBounds = c->getStaticBounds();
//                    if (aabbTest(lineBounds,shapeBounds)) {
//                        const auto& t = c->getNode()->getWorldMatrix();
//                        // if aabb intersect, then try to run proper intersection between the shapes (one of which is a seg)
//                        /// TODO restore following code
//                        auto report = m_raycast->raycast(glm::vec3(P.x, P.y, P.z), glm::vec3(P1.x, P1.y, P1.z), c->getShape().get(), c->getNode()->getWorldMatrix());
//
//                        // update output
//                        if (report.collide && (!out.collide || out.length > report.length)) {
//                            out.entity = c;
//                            out.length = report.length;
//                            out.collide = true;
//                            out.normal = report.normal;
//                        }
//                    }
//                }
//            }
//        }
//        P = P2;
//        i += id;
//        j += jd;
//        k += kd;
//    }
//
//    return out;
//
//
//}

// returns true if bounds intersect
//bool CollisionEngine2D::aabbTest(const Bounds &b1, const Bounds &b2) const{
//
//    return !(b1.min.x > b2.max.x || b1.max.x < b2.min.x ||
//             b1.min.y > b2.max.y || b1.max.y < b2.min.y);
//
//
//}
//
//std::vector<ShapeCastHit> CollisionEngine::shapeCast (Shape* shape, const glm::mat4& transform, int mask, bool onlyFirst) {
//    std::vector<ShapeCastHit> result;
//    auto aabb = shape->getBounds();
//    aabb.transform(transform);
//    //float z = transform[3][2];
//    auto loc = getLocation(aabb);
//    for (int i = loc.first.x; i <= loc.second.x; ++i) {
//        for (int j = loc.first.y; j <= loc.second.y; ++j) {
//            for (int k = loc.first.z; k <= loc.second.z; ++k) {
//                auto cell = m_cells.find(glm::vec3(i, j, k));
//                if (cell != m_cells.end()) {
//                    auto &colliders = cell->second.colliders;
//                    for (auto &c : colliders) {
//                        if (!c->isActive()) {
//                            continue;
//                        }
//                        int flag = c->getCollisionFlag();
//                        int m = flag & mask;
//                        if (m == 0) {
//                            continue;
//                        }
//                        auto b = c->getStaticBounds();
//                        // perform a aabb testing
//                        if (!aabbTest(aabb, b)) {
//                            continue;
//                        }
//                        auto s = c->getShape();
//                        if (s != nullptr) {
//                            const auto &t = c->getNode()->getWorldMatrix();
//                            //auto s1 = s->transform(t);
//                            //auto s2 = shape->transform(transform);
//                            // bounding boxes intersect, so let's make a proper collision test
//                            auto report = m_intersector->intersect(shape, s.get(), transform, t);
//                            if (report.collide) {
//                                Bounds bb = aabb.intersection(b);
//                                ShapeCastHit res;
//                                res.report = report;
//                                res.report.direction = bb.getCenter();
//                                res.entity = c;
//                                result.push_back(res);
//                                if (onlyFirst)
//                                    return result;
//                            }
//                        }
//                    }
//                }
//            }
//        }
//    }
//    return result;
//
//}



//
//const CollisionEngineCell* ICollisionEngine::getColliders(glm::ivec3 pos) {
//    auto it = m_cells.find(pos);
//    if (it == m_cells.end())
//        return nullptr;
//    return &it->second;
//
//}

std::vector<ShapeCastHit> SpatialHashingCollisionEngine::shapeCast(Shape * shape, const glm::mat4 &transform, int mask,
                                                                   bool onlyFirst) {
    // first get the transformed aabb
    auto aabb = shape->getBounds();
    aabb.transform(transform);
    //std::cout << aabb.getSize().x << ", " << aabb.getSize().y << "\n";

    auto loc = getLocation(aabb);
    for (int iy = loc[2]; iy <= loc[3]; iy++) {
        for (int ix = loc[0]; ix <= loc[1]; ix++) {
            auto iter = _colliders.find({ix, iy});
            if (iter != _colliders.end()) {
                for (const auto& c : iter->second) {
                    if (mask != 0 && c->getCollisionFlag() != mask) continue;
                    // test collision between the two shapes
                    // first do a rough bounding box check
                    auto otherBounds = c->getStaticBounds();
                    if (otherBounds.intersect2D(aabb)) {
                        // do a proper collision check
                        auto collide = _intersector.intersect(shape, c->getShape().get(), transform, c->getNode()->getWorldMatrix());
                        if (collide.collide) {
                            ShapeCastHit hit;
                            hit.entity = c;
                            hit.report = collide;
                            return {hit};
                        }
                        //std::cout << "BROAD collision; CHECK = " << collide.collide << std::endl;
                    }
                }
            }
        }
    }


    return {};
}
//void CollisionEngine::processCollisions(const std::vector<ShapeCastHit> & e, Node* node, int tag) {
//
//    if (m_responseManager == nullptr) {
//        return;
//    }
//
//    for (const auto& coll : e) {
//        auto currentNode = coll.entity->getNode();
//        if (m_responseManager->hasCollision(tag, coll.entity->getCollisionTag())) {
//            m_responseManager->onStart(node, currentNode, tag, coll.entity->getCollisionTag(), coll.report.direction);
//        }
//
//    }
//
//}