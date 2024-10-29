#include "aabb.h"

using namespace shapes;

AABB::AABB(float xm, float xM, float ym, float yM) : _xMin(xm), _xMax(xM), _yMin(ym), _yMax(yM) {
    m_points[0] = glm::vec2(xm, ym);
    m_points[1] = glm::vec2(xM, ym);
    m_points[2] = glm::vec2(xM, yM);
    m_points[3] = glm::vec2(xm, yM);
    m_bounds.min = glm::vec3(xm, ym, 0.f);
    m_bounds.max = glm::vec3(xM, yM, 0.f);
    center = 0.5f * (m_points[0] + m_points[2]);
    halfDiag = 0.5f * sqrt((xM-xm)*(xM-xm) + (yM-ym)*(yM-ym));
    m_type = ShapeType::AABB;
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

bool AABB::isInside(glm::vec2 P) const {
    return (P.x >= m_points[0].x && P.x <= m_points[1].x && P.y >= m_points[0].y && P.y <= m_points[2].y);
}

RaycastResult AABB::raycast(glm::vec2 P0, glm::vec2 P1) const {
    // check if ray is horizontal or vertical
    if (P1.x - P0.x == 0.f) {
        // vertical ray
        if (P0.x < _xMin || P0.x > _xMax) {
            return RaycastResult();
        }
        if (P0.y <= _yMin && P1.y > _yMin) {
            return RaycastResult(true, _yMin - P0.y, glm::vec2(0.f, -1.f));
        } else if (P0.y >= _yMax && P1.y < _yMax) {
            return RaycastResult(true, P0.y - _yMax, glm::vec2(0.f, 1.f));
        }
    } else if (P1.y - P0.y == 0.f) {
        // horizontal ray
        if (P0.y < _yMin || P0.y > _yMax) {
            return RaycastResult();
        }
        if (P0.x <= _xMin && P1.x > _xMin) {
            return RaycastResult(true, _xMin - P0.x, glm::vec2(-1.f, 0.f));
        } else if (P0.x >= _xMax && P1.x < _xMax) {
            return RaycastResult(true, P0.x - _xMax, glm::vec2(1.f, 0.f));
        }
    }
    // ray is slanted ... use the general algorithm
    if (P0.y >= _yMax) {
        if (P1.y >= _yMax) {
            return RaycastResult();
        }
        // check intersection with y = ymax
        float u{0.f};
        if (test_y(P0, P1, _yMax, u)) {
            return RaycastResult(true, u * glm::length(P1-P0), glm::vec2(0.f, 1.f));
        }
        if (P0.x < _xMin) {
            // try collide with x = xmin
            if (test_x(P0, P1, _xMin, u)) {
                return RaycastResult(true, u * glm::length(P1-P0), glm::vec2(-1.f, 0.f));
            }
        } else if (P0.x > _xMax) {
            if (test_x(P0, P1, _xMax, u)) {
                return RaycastResult(true, u * glm::length(P1-P0), glm::vec2(1.f, 0.f));
            }
        }
    } else if (P0.y <= _yMin) {
        if (P1.y <= _yMin) {
            return RaycastResult();
        }
        // check intersection with y = ymin
        float u{0.f};
        if (test_y(P0, P1, _yMin, u)) {
            return RaycastResult(true, u * glm::length(P1-P0), glm::vec2(0.f, -1.f));
        }
        if (P0.x < _xMin) {
            // try collide with x = xmin
            if (test_x(P0, P1, _xMin, u)) {
                return RaycastResult(true, u * glm::length(P1-P0), glm::vec2(-1.f, 0.f));
            }
        } else if (P0.x > _xMax) {
            if (test_x(P0, P1, _xMax, u)) {
                return RaycastResult(true, u * glm::length(P1-P0), glm::vec2(1.f, 0.f));
            }
        }
    } else {
        // y is in [_yMin, _yMax]
        float u{0.f};
        if (P0.x < _xMin) {
            if (test_x(P0, P1, _xMin, u)) {
                return RaycastResult(true, u * glm::length(P1-P0), glm::vec2(-1.f, 0.f));
            }
        } else {
            if (test_x(P0, P1, _xMax, u)) {
                return RaycastResult(true, u * glm::length(P1-P0), glm::vec2(1.f, 0.f));
            }
        }
    }
    return RaycastResult();
}

bool AABB::test_x(glm::vec2 P0, glm::vec2 P1, float x, float& u) const {
    u = (x - P0.x) / (P1.x - P0.x);
    auto y_xm = P0.y + u * (P1.y - P0.y);
    if (y_xm >= _yMin && y_xm <= _yMax) {
        return true;
    }
    return false;
}

bool AABB::test_y(glm::vec2 P0, glm::vec2 P1, float y, float& u) const {
    u = (y - P0.y) / (P1.y - P0.y);
    auto xy = P0.x + u * (P1.x - P0.x);
    if (xy >= _xMin && xy <= _xMax) {
        return true;
    }
    return false;
}

