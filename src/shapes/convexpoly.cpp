#include "convexpoly.h"


ConvexPoly::ConvexPoly(const py::array_t<float>& input) {
    glm::vec2 previous;
    m_bounds.min.x = input.at(0);
    m_bounds.max.x = m_bounds.min.x;
    m_bounds.min.y = input.at(1);
    m_bounds.max.y = m_bounds.min.y;
    for (size_t i = 0; i < input.size(); i+=2) {
        glm::vec2 current(input.at(i), input.at(i+1));
        addPoint(current);
//        m_points.push_back(current);
//        m_bounds.addPoint(current);
//        if (i > 0) {
//            addEdge(previous, current);
//        }
//        previous = current;
    }
    closeLoop();
	//addEdge(m_points.back(), m_points.front());
}

void ConvexPoly::addPoint(glm::vec2 P) {
	m_bounds.addPoint(P);
	if (!m_points.empty()) {
		addEdge(m_points.back(), P);
	}
	m_points.push_back(P);
}

void ConvexPoly::closeLoop() {
	addEdge(m_points.back(), m_points.front());
}

void ConvexPoly::addEdge(glm::vec2 &A, glm::vec2 &B) {
	_segments.push_back(Seg{A, B});
    auto edge = B - A;
    //m_edges.push_back(edge);
    // unit vector
    edge = glm::normalize(edge);
    // rotating 90 clockwise
    m_normals.emplace_back(edge.y, -edge.x);



}

glm::vec2 ConvexPoly::project(glm::vec2 axis, const glm::mat4 & t) const {

    glm::vec2 out (std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity());
    for (const auto& p : m_points) {
        glm::vec2 pw = t * glm::vec4(p, 0.0f, 1.0f);
        float pr = glm::dot (pw, axis);
        out.x = std::min (out.x, pr);
        out.y = std::max (out.y, pr);
    }
    return out;
}

Segment::Segment(float x0, float y0, float x1, float y1) {
	addPoint(glm::vec2(x0, y0));
	addPoint(glm::vec2(x1, y1));
}

Rect::Rect(float w, float h, const py::kwargs& kwargs) {
    float ox{0.0f};
    float oy{0.0f};
    if (kwargs) {
        if (kwargs.contains("ox")) {
            ox = kwargs["ox"].cast<float>();
        }
        if (kwargs.contains("oy")) {
            oy = kwargs["oy"].cast<float>();
        }
    }
    addPoint(glm::vec2(ox, oy));
	addPoint(glm::vec2(ox + w, oy));
	addPoint(glm::vec2(ox + w, oy + h));
	addPoint(glm::vec2(ox, oy + h));

}

const std::vector<glm::vec2> & ConvexPoly::getPoints() const {
    return m_points;
}

const std::vector<glm::vec2> & ConvexPoly::getUnitNormals() const {
    return m_normals;
}