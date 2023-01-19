#pragma once

#include <glm/glm.hpp>

class Collider;

struct CollisionReport {
    CollisionReport() : collide(false) {}
    // tells whether a collision has been found
    bool collide;
    float distance;
    glm::vec3 direction;
};

struct RayCastHit {
    RayCastHit() : collide(false), length(std::numeric_limits<float>::infinity()), entity(nullptr), segmentIndex(-1), normal(0.f) {}
    //RayCastHit(bool collide, float l, glm::vec3 normal) : collide(collide), length(l), entity(nullptr), normal(normal), segmentIndex(-1) {}
    bool collide;
    float length;
    Collider* entity;
    glm::vec3 normal;
    int segmentIndex;
};

struct ShapeCastHit {
    ShapeCastHit () : entity(nullptr) {}
    CollisionReport report;
    Collider* entity;


};