#pragma once

#include <pybind11/pybind11.h>
#include "../components/collider.h"
#include "../hashpair.h"



struct CollisionResponse {
    pybind11::function onStart;
    pybind11::function onStay;
    pybind11::function onEnd;
};

class CollisionResponseManager {
public:
    bool hasCollision(Collider*, Collider*);
    bool hasCollision(int, int);
    void add(int, int, const pybind11::kwargs&);
    void onStart(Collider*, Collider*, glm::vec3);
    void onStay(Collider*, Collider*);
    void onEnd(Collider*, Collider*);
    void onStart(Node* node1, Node* node2, int tag1, int tag2, glm::vec3);

private:
    std::unordered_map<std::pair<int, int>, CollisionResponse> m_response;

};

