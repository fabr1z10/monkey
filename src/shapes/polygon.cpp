#include "polygon.h"
#include "../util.h"
#include "../util.h"

using namespace shapes;


Polygon::Polygon(const std::vector<float> &points) {
    m_bounds = Bounds(glm::vec3(points[0], points[1], 0.f));
	for (size_t i = 0; i < points.size(); i+=2) {
		_points.emplace_back(points[i], points[i+1]);
		m_bounds.addPoint(glm::vec3(points[i], points[i+1], 0.f));
	}
    addSegments(_points);
	m_type = ShapeType::POLYGON;

}

RaycastResult Polygon::raycast(glm::vec2 P0, glm::vec2 P1) const {
    const Seg* s = nullptr;
    RaycastResult r;
    for (const auto& seg : _segs) {
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

void Polygon::addSegments(const std::vector<glm::vec2> &p) {
    for (size_t i = 0; i < p.size() - 1; ++i) {
        auto n = glm::normalize(rot90(p[i+1] - p[i]));
        _segs.push_back({p[i], p[i+1], n});
    }
    auto n = glm::normalize(rot90(p.front()-p.back()));
    _segs.push_back({p.back(), p.front(), n});
}

void Polygon::addHole(const std::vector<float> &points) {
	std::vector<glm::vec2> p;
	for (size_t i = 0; i < points.size(); i+=2) {
		p.emplace_back(points[i], points[i+1]);
	}
	_holes.push_back(p);
    addSegments(p);
}

bool Polygon::isInside(glm::vec2 P) const {
	if (pnpoly(_points, P)) {
		for (const auto& hole : _holes) {
			if (pnpoly(hole, P)) {
				return false;
			}
		}
		return true;
	} else {
		return false;
	}
}

// returns whether AB intersects polygon
bool Polygon::intersectSegment(glm::vec2 A, glm::vec2 B, float &t) {
	for (size_t i = 1; i < _points.size(); ++i) {
		float t{0.f};
		if (seg2seg(A, B, _points[i-1], _points[i], t)) {
			return true;
		}
	}

	// check holes
	for (size_t j = 0; j < _holes.size(); ++j) {
		for (size_t i = 1; i < _holes[i].size(); ++i) {
			float t{0.f};
			if (seg2seg(A, B, _points[i-1], _points[i], t)) {
				return true;
			}
		}
	}


	// check mid point. The segment might connect two vertices of the polygon but lie completely outside of it
	// quick way is to check if mid point of the segment lies inside the poly
	glm::vec2 mid = (A + B) * 0.5f;
	if (isInside(glm::vec3(mid, 0.f))) {
		return false;
	}
	return true;
}