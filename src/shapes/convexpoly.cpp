#include "convexpoly.h"
#include "../util.h"

using namespace shapes;

ConvexPoly::ConvexPoly() {
    m_type = ShapeType::CONVEX_POLY;
}

ConvexPoly::ConvexPoly(const std::vector<float>& input) {
    m_type = ShapeType::CONVEX_POLY;

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

RaycastResult ConvexPoly::raycast(glm::vec2 P0, glm::vec2 P1) const {
    const Seg* s = nullptr;
    RaycastResult r;
    for (const auto& seg : _segments) {
        float u{0.f};
        if (seg2seg(P0, P1, seg.P0, seg.P1, u) && (s == nullptr || u < r.length)) {
            s = &seg;
            r.length = u;
            r.collide = true;
            r.normal = s->n;
        }
    }
    return r;
}

bool ConvexPoly::isInside(glm::vec2 P) const {
    int pos = 0, neg = 0;
    for (const auto& seg : _segments) {
        auto p = cross2D(seg.P1 - seg.P0, glm::vec2(P) - seg.P0);
        if (p > 0)
            pos++;
        else if (p < 0)
            neg ++;
        if (pos > 0 && neg > 0) return false;
    }
    return true;
}


void ConvexPoly::addPoint(glm::vec2 P) {
    if (m_points.empty()) {
        m_bounds.min = glm::vec3(P, 0.f);
        m_bounds.max = m_bounds.min;
    } else {
        m_bounds.addPoint(P);
    }
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



Rect::Rect(float w, float h, glm::vec2 anchor) : ConvexPoly() {
    glm::vec2 tl = -anchor;
    addPoint(tl);
	addPoint(tl + glm::vec2(w, 0.f));
	addPoint(tl + glm::vec2(w, h));
	addPoint(tl + glm::vec2(0.f, h));

}

const std::vector<glm::vec2> & ConvexPoly::getPoints() const {
    return m_points;
}

const std::vector<glm::vec2> & ConvexPoly::getUnitNormals() const {
    return m_normals;
}

Segment::Segment(float x0, float y0, float x1, float y1) : ConvexPoly() {
    addPoint(glm::vec2(x0, y0));
    addPoint(glm::vec2(x1, y1));
}