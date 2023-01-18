#pragma once

#include "../shape.h"
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <vector>

namespace py = pybind11;

class ConvexPoly : public Shape2D {
public:
    ConvexPoly() = default;
    explicit ConvexPoly(const py::array_t<float>& input);

    glm::vec2 project(glm::vec2, const glm::mat4&) const override;
    const std::vector<glm::vec2>& getPoints() const;
    const std::vector<glm::vec2>& getUnitNormals() const;
protected:
    void addEdge (glm::vec2& A, glm::vec2& B);
    std::vector<glm::vec2> m_points;
    std::vector<glm::vec2> m_edges;
    // unit normals
    std::vector<glm::vec2> m_normals;

};


class Rect : public ConvexPoly {
public:
    Rect(float w, float h, const py::kwargs&);

};


class Segment : public ConvexPoly {
public:
    Segment(float x0, float y0, float x1, float y1);
};


