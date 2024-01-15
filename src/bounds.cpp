#include <algorithm>
#include "bounds.h"


Bounds Bounds::maxBounds() {
	static Bounds maxB(glm::vec3(-std::numeric_limits<float>::max()), glm::vec3(std::numeric_limits<float>::max()));
	return maxB;
}

Bounds::Bounds() : min(glm::vec3(0.0f)), max(glm::vec3(0.0f)) {

}

void Bounds::scale(float, float) {
	auto halfWidth = (max.x - min.x) * 0.5f;
	auto halfHeight = (max.y - min.y) * 0.5f;
	min.x -= halfWidth;
	max.x += halfWidth;
	min.y -= halfHeight;
	max.y += halfHeight;
}

void Bounds::reset() {
    min = glm::vec3(0.f);
    max = glm::vec3(0.f);

}
Bounds::Bounds(glm::vec3 m, glm::vec3 M) : min(m), max(M) {}

Bounds::Bounds(glm::vec3 m) : min(m), max(m) {}

void Bounds::addPoint(glm::vec2 P) {
    min.x = std::min(min.x, P.x);
    max.x = std::max(max.x, P.x);
    min.y = std::min(min.y, P.y);
    max.y = std::max(max.y, P.y);
}

void Bounds::addPoint(glm::vec3 P) {
    min.x = std::min(min.x, P.x);
    max.x = std::max(max.x, P.x);
    min.y = std::min(min.y, P.y);
    max.y = std::max(max.y, P.y);
    min.z = std::min(min.z, P.z);
    max.z = std::max(max.z, P.z);
}

bool Bounds::isVoid() const {
    return (max.x == min.x && max.y == min.y && max.z == min.z);
}

bool Bounds::intersect2D(Bounds& other) const {
    return !(other.min.x > max.x || other.max.x < min.x ||
             other.min.y > max.y || other.max.y < min.y);
}

bool Bounds::intersect3D(Bounds& other) const {
    return !(other.min.x > max.x || other.max.x < min.x ||
             other.min.y > max.y || other.max.y < min.y ||
             other.min.z > max.z || other.max.z < min.z);
}

void Bounds::translate(const glm::vec3& vec) {
	min += vec;
	max += vec;

}

void Bounds::transform(const glm::mat4& m) {
    glm::vec3 P[] = {
            glm::vec3(min.x, min.y, min.z),
            glm::vec3(max.x, min.y, min.z),
            glm::vec3(max.x, max.y, min.z),
            glm::vec3(min.x, max.y, min.z),
            glm::vec3(min.x, min.y, max.z),
            glm::vec3(max.x, min.y, max.z),
            glm::vec3(max.x, max.y, max.z),
            glm::vec3(min.x, max.y, max.z),
    };

    float inf = std::numeric_limits<float>::infinity();
    min = glm::vec3(inf, inf, inf);
    max = glm::vec3(-inf, -inf, -inf);
    for (int i = 0; i < 8; i++) {
        glm::vec3 Pt = glm::vec3(m * glm::vec4(P[i], 1.0f));
        if (Pt.x < min.x)
            min.x = Pt.x;
        if (Pt.x > max.x)
            max.x = Pt.x;
        if (Pt.y < min.y)
            min.y = Pt.y;
        if (Pt.y > max.y)
            max.y = Pt.y;
        if (Pt.z < min.z)
            min.z = Pt.z;
        if (Pt.z > max.z)
            max.z = Pt.z;
    }
}


void Bounds::expandWith(const Bounds & b) {
    if (isVoid()) {
        min = b.min;
        max = b.max;
    } else {
        min.x = std::min(min.x, b.min.x);
        min.y = std::min(min.y, b.min.y);
        max.x = std::max(max.x, b.max.x);
        max.y = std::max(max.y, b.max.y);
    }
}

Bounds Bounds::intersection(const Bounds &other) {
    Bounds b;
    b.min.x = std::max(this->min.x, other.min.x);
    b.min.y = std::max(this->min.y, other.min.y);
    b.max.x = std::min(this->max.x, other.max.x);
    b.max.y = std::min(this->max.y, other.max.y);
    return b;

}

glm::vec3 Bounds::getCenter() {
    return 0.5f * (min + max);
}