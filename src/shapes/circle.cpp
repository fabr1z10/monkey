#include "circle.h"
#include "../pyhelper.h"

Circle::Circle(float radius, const py::kwargs& args) : Shape2D(), m_radius(radius) {
    auto ox = py_get_dict<float>(args, "ox", 0.0f);
    auto oy = py_get_dict<float>(args, "oy", 0.0f);
    m_offset = glm::vec3(ox, oy, 0.f);
    m_bounds.min = glm::vec3(m_offset.x - m_radius, m_offset.y - m_radius, 0.f);
    m_bounds.max = glm::vec3(m_offset.x + m_radius, m_offset.y + m_radius, 0.f);


}

glm::vec2 Circle::project(glm::vec2 axis, const glm::mat4& t) const {
    glm::vec2 centerWorld = t * glm::vec4(m_offset, 1.0f);
    float x = glm::dot(centerWorld, axis);
    return glm::vec2(x - m_radius, x + m_radius);
}
