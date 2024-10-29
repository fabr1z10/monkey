#include "shape.h"

using namespace shapes;

bool Shape::rayOutsideBounds(glm::vec2 P0, glm::vec2 P1) const {
    return (P0.x < m_bounds.min.x && P1.x < m_bounds.min.x) ||
            (P0.x > m_bounds.max.x && P1.x > m_bounds.max.x) ||
            (P0.y < m_bounds.min.y && P1.y < m_bounds.min.y) ||
            (P0.y > m_bounds.max.y && P1.y > m_bounds.max.y);
}