#include "aabb.h"


AABB::AABB(float xm, float xM, float ym, float yM) : m_xMin(xm), m_xMax(xM), m_yMin(ym), m_yMax(yM) {
    m_points[0] = glm::vec2(xm, ym);
    m_points[1] = glm::vec2(xM, ym);
    m_points[2] = glm::vec2(xM, yM);
    m_points[3] = glm::vec2(xm, yM);
    m_bounds.min = glm::vec3(xm, ym, 0.f);
    m_bounds.max = glm::vec3(xM, yM, 0.f);
    center = 0.5f * (m_points[0] + m_points[2]);
    halfDiag = 0.5f * sqrt((xM-xm)*(xM-xm) + (yM-ym)*(yM-ym));
    m_type = ShapeType::AABB2D;
}

glm::vec2 AABB::project(glm::vec2 axis, const glm::mat4 & t) const {
    auto offset = glm::vec2(t[3]);
    glm::vec2 out (std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity());
    for (const auto& p : m_points) {
        glm::vec2 pw = offset + p;
        float pr = glm::dot (pw, axis);
        out.x = std::min (out.x, pr);
        out.y = std::max (out.y, pr);
    }
    return out;
}

bool AABB::isInside(glm::vec3 P) const {
    return (P.x >= m_points[0].x && P.x <= m_points[1].x && P.y >= m_points[0].y && P.y <= m_points[2].y);
}