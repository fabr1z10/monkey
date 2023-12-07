#pragma once

#include "../shape.h"
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <vector>

namespace py = pybind11;

class ConvexPoly : public ConvexShape {
public:
    ConvexPoly() = default;
    explicit ConvexPoly(const py::array_t<float>& input);

    glm::vec2 project(glm::vec2, const glm::mat4&) const override;
    const std::vector<glm::vec2>& getPoints() const;
    const std::vector<glm::vec2>& getUnitNormals() const;
	const std::vector<Seg>* getSegments() const override;
protected:
	// add point may call addedge which in turn adds the segment, the normal and update bounds
	void addPoint(glm::vec2);
    void addEdge (glm::vec2& A, glm::vec2& B);
	void closeLoop();
    std::vector<glm::vec2> m_points;
    std::vector<Seg> _segments;
    //std::vector<glm::vec2> m_edges;
    // unit normals
    std::vector<glm::vec2> m_normals;

};

inline const std::vector<Seg> * ConvexPoly::getSegments() const {
	return &_segments;
}


class Rect : public ConvexPoly {
public:
    Rect(float w, float h, const py::kwargs&);

};


class Segment : public ConvexPoly {
public:
    Segment(float x0, float y0, float x1, float y1);
};




