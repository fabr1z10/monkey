#include "circle.h"
#include "glm/gtx/norm.hpp"


using namespace shapes;


Circle::Circle(float radius, glm::vec2 center) : ConvexShape(), m_radius(radius), _r2(radius*radius) {
    m_type = ShapeType::CIRCLE;
    _center = center;
    m_bounds.min = glm::vec3(_center.x - m_radius, _center.y - m_radius, 0.f);
    m_bounds.max = glm::vec3(_center.x + m_radius, _center.y + m_radius, 0.f);


}

glm::vec2 Circle::project(glm::vec2 axis, const glm::mat4& t) const {
    glm::vec2 centerWorld = t * glm::vec4(_center, 0.f, 1.0f);
    float x = glm::dot(centerWorld, axis);
    return glm::vec2(x - m_radius, x + m_radius);
}

bool Circle::isInside(glm::vec2 P ) const {
    return glm::distance2(glm::vec2(P), _center) <= _r2;

}

RaycastResult Circle::raycast(glm::vec2 P0, glm::vec2 P1) const {
    RaycastResult r;
    if (rayOutsideBounds(P0, P1)) return r;

    bool P0in = isInside(P0);

    if (!P0in) {
        glm::vec2 u = glm::normalize(P1-P0);
        float a = glm::dot(_center - P0, u);
        float E2 = glm::distance2(_center, P0);
        float l = a - sqrt(_r2 - E2 + a*a);
        glm::vec2 hitPoint = P0 + l * u;
        r.normal = glm::normalize(hitPoint - _center);
        r.length = l;
        r.collide = true;
        return r;
    }
    return r;
}
