#include <iostream>
#include "collision_engine.h"
#include "../node.h"
#include "../upair.h"
#include "../shapes/convexpoly.h"
#include "../shapes/intersector2d.h"
#include "../shapes3d/seg3d.h"

// to specify a 2D engine, just set depth to 0
CollisionEngine::CollisionEngine(float width, float height, float depth) : m_size(width, height, depth) {
    assert(width > 0.f);
    assert(height > 0.f);
    assert(depth >= 0.f);
    m_2d = depth == 0.f;
    m_responseManager = std::make_shared<CollisionResponseManager>();

    if (m_2d) {
        m_intersector = std::make_shared<Intersector2D>();
        m_raycast = std::make_shared<RayCaster2D>();
    } else {
        //m_intersector = std::make_shared<Intersector3D>();
        //m_raycast = std::make_shared<RayCaster3D>();

    }
}

void CollisionEngine::add(Collider * c) {
    // this is called when a new collider starts. It registers with the engine
    // get the shape bounding box, transform it, map it
    auto aabb = c->getStaticBounds();
    if (!aabb.isVoid()) {
        auto loc = getLocation(aabb);
        pushCollider(c, loc.first, loc.second);
    }
}

void CollisionEngine::remove(Collider * c) {
    m_removed.insert(c);
    auto d = m_colliderLocations.at(c);
    for (auto i = d.min.x; i <= d.max.x; ++i) {
        for (auto j = d.min.y; j <= d.max.y; ++j) {
            for (auto k = d.min.z; k <= d.max.z; ++k) {
                auto aa = m_cells.find(glm::vec3(i, j, k));
                if (aa != m_cells.end()) {
                    aa->second.colliders.erase(c);
                }
            }
        }
    }
    m_colliderLocations.erase(c);
    for (auto it = m_previouslyCollidingPairs.begin(); it != m_previouslyCollidingPairs.end(); ) {
        if (it->first.first == c || it->first.second == c) {
            it = m_previouslyCollidingPairs.erase(it);
        } else {
            it++;
        }
    }
}

void CollisionEngine::move(Collider * c) {
    // TODO mark collider as DIRTY, and update collider position (cells occupied)
    // then at update time, go thorugh each dirty collider, and test it with other colliders in cells
    // to avoid double testing, keep track of tested pairs
    auto aabb = c->getStaticBounds();
    auto loc = getLocation(aabb);
    pushCollider(c, loc.first, loc.second);			// this automatically sets it to dirty
    //std::cout << "aabb: (" << aabb.min.x << ", " << aabb.min.y << "), (" << aabb.max.x << ", " << aabb.max.y << ")\n";
}

void CollisionEngine::update(double) {

    std::unordered_map<std::pair<Collider*, Collider*>, CollisionInfo> currentlyCollidingPairs;
    std::unordered_set<glm::ivec3> cellsExamined;

    unsigned collisionChecks = 0;
    std::unordered_set<UPair<Collider*>, UPair<Collider*>::HashFunction> processed;
    for (auto& c : m_colliderLocations) {
        if (c.second.dirty) {
            auto *c1 = c.first;
            // loop through cells occupied by collider
            for (auto i = c.second.min.x; i <= c.second.max.x; ++i) {
                for (auto j = c.second.min.y; j <= c.second.max.y; ++j) {
                    for (auto k = c.second.min.z; k <= c.second.max.z; ++k) {
                        // doing cell (i, j, k)
                        const auto &colliders = m_cells.at(glm::ivec3(i, j, k)).colliders;
                        if (colliders.size() <= 1) {
                            continue;
                        }
                        for (const auto &c2 : colliders) {
                            if (m_removed.count(c2) > 0) {
                                //std::cout << "HEY\n";
                            }
                            // don't collide with itself
                            auto pair = UPair<Collider *>(c1, c2);
                            if (processed.find(pair) != processed.end()) continue;
                            processed.insert(UPair<Collider *>(c1, c2));
                            if (c1 == c2 || !c2->isActive()) continue;
                            if (((c1->getCollisionMask() & c2->getCollisionFlag()) == 0) &&
                                ((c2->getCollisionMask() & c1->getCollisionFlag()) == 0)) {
                                continue;
                            }
                            // if no response is provided for these tags, then skip it
                            if (m_responseManager == nullptr || !m_responseManager->hasCollision(c1, c2)) {
                                continue;
                            }
                            // we have a collision response, so let's calculate collision
                            collisionChecks++;
                            auto b1 = c1->getStaticBounds();
                            auto b2 = c2->getStaticBounds();
                            if (!aabbTest(b1, b2)) {
                                continue;
                            }

                            // get the shape in local coordinates
                            const auto &t1 = c1->getNode()->getWorldMatrix();
                            const auto &t2 = c2->getNode()->getWorldMatrix();

                            auto report = m_intersector->intersect(c1->getShape().get(), c2->getShape().get(), t1, t2);
                            if (report.collide) {
                                CollisionInfo ci;
                                ci.report = report;
                                ci.pos = glm::vec3(i, j, k);
                                currentlyCollidingPairs.insert(std::make_pair(std::make_pair(c1, c2), ci));
                            }
                        }
                    }
                }
            }
        }

        c.second.dirty = false;

    }
    //std::cout << "collision checks: " << collisionChecks << std::endl;

    // remove pairs that were previously colliding but not now
    for (auto iter = m_previouslyCollidingPairs.begin(); iter != m_previouslyCollidingPairs.end();) {
        CollisionInfo& ci = iter->second;
        // If i have examined the cell AND they are not colliding anymore ...
        if (processed.count(UPair(iter->first.first, iter->first.second)) > 0 && currentlyCollidingPairs.count(iter->first) == 0) {
            m_responseManager->onEnd (iter->first.first, iter->first.second);
            m_previouslyCollidingPairs.erase(iter++);
        }
        else {
            iter++;
        }
    }

    for (auto& p : currentlyCollidingPairs) {
        auto it = m_previouslyCollidingPairs.find(p.first);
        if (it == m_previouslyCollidingPairs.end()) {
            m_responseManager->onStart (p.first.first, p.first.second, p.second.report.direction * p.second.report.distance);
            m_previouslyCollidingPairs.insert(std::make_pair(p.first, p.second));
        } else {
            m_responseManager->onStay(p.first.first, p.first.second);
            it->second = p.second;
        }
    }
}

void CollisionEngine::pushCollider(Collider* c, glm::ivec3 m, glm::ivec3 M) {
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



std::pair<glm::ivec3, glm::ivec3> CollisionEngine::getLocation(const Bounds &b) {
    glm::ivec3 min(0);
    glm::ivec3 max(0);
    min.x = getIndex(b.min.x, m_size[0]);
    min.y = getIndex(b.min.y, m_size[1]);
    min.z = getIndex(b.min.z, m_size[2]);
    max.x = getIndex(b.max.x, m_size[0]);
    max.y = getIndex(b.max.y, m_size[1]);
    max.z = getIndex(b.max.z, m_size[2]);
    return std::make_pair(min, max);
}

int CollisionEngine::getIndex(float x, float s) {
    return s == 0.f ? 0 : (-1 * (x < 0) + static_cast<int>(x / s));
}

void CollisionEngine::addResponse(int i, int j, const pybind11::kwargs& args) {
    m_responseManager->add(i, j, args);
}

RayCastHit CollisionEngine::rayCast(glm::vec3 rayOrigin, glm::vec3 rayDir, float length, int mask) {
    if (m_2d) {
        rayOrigin.z = 0.f;
        rayDir.z = 0.f;
    }
    glm::vec3 P = rayOrigin;
    glm::vec3 P1 = P;
    glm::vec3 P2 = P;
    //float z = rayOrigin.z;

    // initialize current cell
    int i = getIndex(P.x, m_size[0]);
    int j = getIndex(P.y, m_size[1]);
    // for 2D coll engine --> k must always be 0 ?
    int k = getIndex(P.z, m_size[2]);			// for 2D m_size[2] == 0 --> k = 0

    int n = (rayDir.x > 0 ? 1 : 0);
    int m = (rayDir.y > 0 ? 1 : 0);
    int q = (rayDir.z > 0 ? 1 : 0);

    // length traversed so far
    float l = 0.0f;
    bool endReached = false;

    // increments
    int id = 0, jd = 0, kd = 0;
    RayCastHit out;
    out.length = length;

    // we can (and we MUST) exit the loop as soon as we find a collision
    glm::ivec3 increments(rayDir.x > 0 ? 1 : -1, rayDir.y > 0 ? 1 : -1, rayDir.z > 0 ? 1 : -1);
    while (!endReached && !out.collide) {

        // gets the unit increments along the different axis to hit the cell boundary
        // tx , ty and tz should always be positive: if say raidir.x < 0 also the numerator should be neg
        // this allow us to see what is the next cell to examine
        id = increments.x;
        jd = 0;
        kd = 0;
        float tx = (rayDir.x == 0.0f) ? std::numeric_limits<float>::infinity() : ((i+n) * m_size.x - P.x) / rayDir.x;
        float tbest = tx;
        float ty = (rayDir.y == 0.0f) ? std::numeric_limits<float>::infinity() : ((j+m) * m_size.y - P.y) / rayDir.y;
        if (ty <= tbest) {
            id = 0;
            jd = increments.y;
            tbest = ty;
        }
        // for 2D tz must always be inf
        float tz = (rayDir.z == 0.0f) ? std::numeric_limits<float>::infinity() : ((k+q) * m_size.z - P.z) / rayDir.z;
        if (tz <= tbest) {
            id = 0;
            jd = 0;
            kd = increments.z;
            tbest = tz;
        }

        float tm {0.0f};
        id = 0;
        jd = 0;
        kd = 0;
        if (tx <= ty) {
            tm = tx;
            id = rayDir.x > 0 ? 1 : -1;
        } else {
            tm = ty;
            jd = rayDir.y > 0 ? 1 : -1;
        }

        // advance by tm TODO are u sure?
        if (l + tm < length) {
            // need to add a tiny extra bit in case the colliding object is a line that lies exactly at the border
            // of two neighboring cell!
            P1 = P + (tm + 0.01f) * rayDir;
            P2 = P + (tm - 0.01f) * rayDir;
            // add tm to the cumulated length done
            l += tm;
        } else {
            P1 = P + (length - l) * rayDir;
            endReached = true;
        }

        // get the colliders at the current cell
        auto it = m_cells.find(glm::ivec3(i, j, k));
        Segment3D line(P, P1);
        auto lineBounds = line.getBounds();
        if (it != m_cells.end()) {
            for (auto& c : it->second.colliders) {
                if (!c->isActive())
                    continue;
                // aabb check
                int flag = c->getCollisionFlag();
                int fm = flag & mask;
                if (fm != 0) {
                    auto shapeBounds = c->getStaticBounds();
                    if (aabbTest(lineBounds,shapeBounds)) {
                        const auto& t = c->getNode()->getWorldMatrix();
                        // if aabb intersect, then try to run proper intersection between the shapes (one of which is a seg)
                        /// TODO restore following code
                        auto report = m_raycast->raycast(glm::vec3(P.x, P.y, P.z), glm::vec3(P1.x, P1.y, P1.z), c->getShape().get(), c->getNode()->getWorldMatrix());

                        // update output
                        if (report.collide && (!out.collide || out.length > report.length)) {
                            out.entity = c;
                            out.length = report.length;
                            out.collide = true;
                            out.normal = report.normal;
                        }
                    }
                }
            }
        }
        P = P2;
        i += id;
        j += jd;
        k += kd;
    }

    return out;


}

// returns true if bounds intersect
bool CollisionEngine::aabbTest(const Bounds &b1, const Bounds &b2) {

    return !(b1.min.x > b2.max.x || b1.max.x < b2.min.x ||
             b1.min.y > b2.max.y || b1.max.y < b2.min.y ||
             (!m_2d && (b1.min.z > b2.max.z || b1.max.z < b2.min.z)));

}

std::vector<ShapeCastHit> CollisionEngine::shapeCast (Shape* shape, const glm::mat4& transform, int mask, bool onlyFirst) {
    std::vector<ShapeCastHit> result;
    auto aabb = shape->getBounds();
    aabb.transform(transform);
    //float z = transform[3][2];
    auto loc = getLocation(aabb);
    for (int i = loc.first.x; i <= loc.second.x; ++i) {
        for (int j = loc.first.y; j <= loc.second.y; ++j) {
            for (int k = loc.first.z; k <= loc.second.z; ++k) {
                auto cell = m_cells.find(glm::vec3(i, j, k));
                if (cell != m_cells.end()) {
                    auto &colliders = cell->second.colliders;
                    for (auto &c : colliders) {
                        if (!c->isActive()) {
                            continue;
                        }
                        int flag = c->getCollisionFlag();
                        int m = flag & mask;
                        if (m == 0) {
                            continue;
                        }
                        auto b = c->getStaticBounds();
                        // perform a aabb testing
                        if (!aabbTest(aabb, b)) {
                            continue;
                        }
                        auto s = c->getShape();
                        if (s != nullptr) {
                            const auto &t = c->getNode()->getWorldMatrix();
                            //auto s1 = s->transform(t);
                            //auto s2 = shape->transform(transform);
                            // bounding boxes intersect, so let's make a proper collision test
                            auto report = m_intersector->intersect(shape, s.get(), transform, t);
                            if (report.collide) {
                                Bounds bb = aabb.intersection(b);
                                ShapeCastHit res;
                                res.report = report;
                                res.report.direction = bb.getCenter();
                                res.entity = c;
                                result.push_back(res);
                                if (onlyFirst)
                                    return result;
                            }
                        }
                    }
                }
            }
        }
    }
    return result;

}




const CollisionEngineCell* CollisionEngine::getColliders(glm::ivec3 pos) {
    auto it = m_cells.find(pos);
    if (it == m_cells.end())
        return nullptr;
    return &it->second;

}

void CollisionEngine::processCollisions(const std::vector<ShapeCastHit> & e, Node* node, int tag) {

    if (m_responseManager == nullptr) {
        return;
    }

    for (const auto& coll : e) {
        auto currentNode = coll.entity->getNode();
        if (m_responseManager->hasCollision(tag, coll.entity->getCollisionTag())) {
            m_responseManager->onStart(node, currentNode, tag, coll.entity->getCollisionTag(), coll.report.direction);
        }

    }

}