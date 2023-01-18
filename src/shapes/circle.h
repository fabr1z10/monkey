#pragma once

#include "../shape.h"
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <vector>

namespace py = pybind11;

class Circle : public Shape2D {
public:
    explicit Circle(float radius, const py::kwargs&);
    glm::vec2 project(glm::vec2, const glm::mat4&) const override;
    float getRadius() const;
protected:
    float m_radius;
};

inline float Circle::getRadius() const {
    return m_radius;
}
