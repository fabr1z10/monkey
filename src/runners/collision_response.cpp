#include "collision_response.h"
#include "../pyhelper.h"
#include "../node.h"

bool CollisionResponseManager::hasCollision(Collider * a, Collider * b) {
    int at = a->getCollisionTag();
    int bt = b->getCollisionTag();
    return m_response.count(std::make_pair(at, bt)) > 0 || m_response.count(std::make_pair(bt, at));
}


bool CollisionResponseManager::hasCollision(int a, int b) {
    return m_response.count(std::make_pair(a, b)) > 0 || m_response.count(std::make_pair(b, a)) > 0;
}

void CollisionResponseManager::add(int i , int j, const pybind11::kwargs& f) {

    CollisionResponse resp;
    resp.onStart = py_get_dict<pybind11::function>(f, "on_start", pybind11::function());
    resp.onStay = py_get_dict<pybind11::function>(f, "on_stay", pybind11::function());
    resp.onEnd = py_get_dict<pybind11::function>(f, "on_end", pybind11::function());
    m_response[std::make_pair(i, j)] = resp;
}

void CollisionResponseManager::onStart(Collider * c1, Collider * c2, glm::vec3 v) {
    auto handler = m_response.find(std::make_pair(c1->getCollisionTag(), c2->getCollisionTag()));
    if (handler != m_response.end() && handler->second.onStart) {
        handler->second.onStart(c1->getNode(), c2->getNode(), pybind11::make_tuple(v.x, v.y, v.z));
    } else {
        handler = m_response.find(std::make_pair(c2->getCollisionTag(), c1->getCollisionTag()));
        if (handler != m_response.end() && handler->second.onStart) {
            handler->second.onStart(c2->getNode(), c1->getNode(), pybind11::make_tuple(-v.x, -v.y, -v.z));
        }
    }
}

void CollisionResponseManager::onStart(Node* n1, Node* n2, int tag1, int tag2, glm::vec3 v) {
    auto handler = m_response.find(std::make_pair(tag1, tag2));
    if (handler != m_response.end() && handler->second.onStart) {
        handler->second.onStart(n1, n2, v);
    } else {
        handler = m_response.find(std::make_pair(tag2, tag1));
        if (handler != m_response.end() && handler->second.onStart) {
            handler->second.onStart(n2, n1, -v);
        }
    }
}

void CollisionResponseManager::onStay(Collider * c1, Collider * c2) {
    auto handler = m_response.find(std::make_pair(c1->getCollisionTag(), c2->getCollisionTag()));
    if (handler != m_response.end() && handler->second.onStay) {
        handler->second.onStay(c1->getNode(), c2->getNode());
    } else {
        handler = m_response.find(std::make_pair(c2->getCollisionTag(), c1->getCollisionTag()));
        if (handler != m_response.end() && handler->second.onStay) {
            handler->second.onStay(c2->getNode(), c1->getNode());
        }
    }
}

void CollisionResponseManager::onEnd(Collider * c1, Collider * c2) {
    auto handler = m_response.find(std::make_pair(c1->getCollisionTag(), c2->getCollisionTag()));
    if (handler != m_response.end() && handler->second.onEnd) {
        handler->second.onEnd(c1->getNode(), c2->getNode());
    } else {
        handler = m_response.find(std::make_pair(c2->getCollisionTag(), c1->getCollisionTag()));
        if (handler != m_response.end() && handler->second.onEnd) {
            handler->second.onEnd(c2->getNode(), c1->getNode());
        }
    }
}