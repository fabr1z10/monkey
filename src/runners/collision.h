#pragma once

#include <glm/glm.hpp>

#include "../math/geometry.h"

class Collider;



struct RayCastHit {
    RayCastHit() : collide(false), length(std::numeric_limits<float>::infinity()), entity(nullptr), segmentIndex(-1), normal(0.f) {}
    RayCastHit(bool collide, float l, glm::vec3 normal) : collide(collide), length(l), entity(nullptr), normal(normal), segmentIndex(-1) {}
    bool collide;
    float length;
    void update(float length, Collider* collider, glm::vec2 normal, int segIndex) {
    	if (!collide || length < this->length) {
    		this->length = length;
    		this->collide = true;
    		this->entity = collider;
    		this->normal = normal;
    		this->segmentIndex = segIndex;
    	}
    }
    Collider* entity;
    glm::vec2 normal;
    int segmentIndex;
};

struct ShapeCastHit {
    ShapeCastHit () : entity(nullptr) {}
    CollisionReport report;
    Collider* entity;


};