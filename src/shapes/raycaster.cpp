#include "raycaster.h"
#include "../shapes/convexpoly.h"
#include "../shapes/aabb.h"
#include "../shapes/polyline.h"
#include "../util.h"

std::vector<glm::ivec3> RayCaster::_axes;

RayCaster::RayCaster() {
	_axes.emplace_back(0, 1, 2);
	_axes.emplace_back(1, 0, 2);
	_axes.emplace_back(2, 0, 1);
}

RayCaster2D::RayCaster2D() {
	m_functionMap[std::type_index(typeid(ConvexPoly))] = [&] (const glm::vec3& A, const glm::vec3& B, const Shape* s, const glm::mat4& t) {
		return rayCastPoly(A, B, s, t);
	};
	m_functionMap[std::type_index(typeid(Rect))] = [&] (const glm::vec3& A, const glm::vec3& B, const Shape* s, const glm::mat4& t) {
		return rayCastPoly(A, B, s, t);
	};
	m_functionMap[std::type_index(typeid(Segment))] = [&] (const glm::vec3& A, const glm::vec3& B, const Shape* s, const glm::mat4& t) {
		return rayCastPoly(A, B, s, t);
	};
    m_functionMap[std::type_index(typeid(AABB))] = [&] (const glm::vec3& A, const glm::vec3& B, const Shape* s, const glm::mat4& t) {
        return rayCastAABB(A, B, s, t);
    };

    /// axis
    m_functionsX[std::type_index(typeid(PolyLine))] = [&] (const glm::vec3& P, float length, const Shape* shape, const glm::mat4& t) { return rayCastXGeneric(P, length, shape, t); };
	m_functionsY[std::type_index(typeid(PolyLine))] = [&] (const glm::vec3& P, float length, const Shape* shape, const glm::mat4& t) { return rayCastYGeneric(P, length, shape, t); };
//    m_functionAxisMap[std::type_index(typeid(ConvexPoly))] = [&] (const glm::vec3 &P, Direction d, float length, const Shape * aabb, const glm::mat4 &t) {
//    	return rayCastAxid2DGeneric(P, d, length, aabb, t);
//	};
//	m_functionAxisMap[std::type_index(typeid(Rect))] = [&] (const glm::vec3 &P, Direction d, float length, const Shape * aabb, const glm::mat4 &t) {
//		return rayCastAxid2DGeneric(P, d, length, aabb, t);
//	};
//	m_functionAxisMap[std::type_index(typeid(Segment))] = [&] (const glm::vec3 &P, Direction d, float length, const Shape * aabb, const glm::mat4 &t) {
//		return rayCastAxid2DGeneric(P, d, length, aabb, t);
//	};
//	m_functionAxisMap[std::type_index(typeid(AABB))] = [&] (const glm::vec3 &P, Direction d, float length, const Shape * aabb, const glm::mat4 &t) {
//		return rayCastAxid2DAABB(P, d, length, aabb, t);
//	};

};

RayCastHit RayCaster2D::rayCastAxid2DAABB(const glm::vec3 &P, Direction d, float length, const Shape * aabb, const glm::mat4 &t) {
	const auto& indices = _axes[d];
	auto i0 = indices[0];
	auto i1 = indices[1];
	auto bounds = aabb->getBounds();
	/// JUST translate bounds, don't rotate them, this is AABB!
	bounds.translate(t[3]);
	RayCastHit out;
	if (length == 0 || (P[i1] > bounds.max[i1] || P[i1] < bounds.min[i1])) {
		return out;
	}

	if (length > 0) {
		if (P[i0] < bounds.min[i0] && P[i0] + length > bounds.min[i0]) {
			return RayCastHit(true, bounds.min[i0] - P[i0], glm::vec3(-1, 0, 0));
		} else if (P[i0] < bounds.max[i0] && P[i0] + length > bounds.max[i0]) {
			return RayCastHit(true, bounds.max[i0] - P[i0], glm::vec3(-1, 0, 0));
		}
	} else  {
		if (P[i0] > bounds.min[i0] && P[i0] - length < bounds.min[i0]) {
			return RayCastHit(true, P[i0] - bounds.min[i0], glm::vec3(1, 0, 0));
		} else if (P[i0] > bounds.max[i0] && P[i0] - length < bounds.max[i0]) {
			return RayCastHit(true, P[i0] - bounds.max[i0], glm::vec3(1, 0, 0));
		}
	}
	return out;

}

RayCastHit RayCaster2D::rayCastXGeneric(const glm::vec3 &P, float length, const Shape * s, const glm::mat4 &t) {
	const auto* s2d = static_cast<const Shape2D*>(s);
	auto* segs = s2d->getSegments();
	float xm = P.x, xM = P.x + length;
	if (length < 0) std::swap(xm, xM);
	RayCastHit out;
	if (segs != nullptr) {
		for (auto& seg : *segs) {
			// transform each point
			auto s0 = t * glm::vec4(seg.P0, 0, 1);
			auto s1 = t * glm::vec4(seg.P1, 0, 1);
			float sxm = s0.x, sxM = s1.x;
			float sym = s0.y, syM = s1.y;
			if (sxm > sxM) std::swap(sxm, sxM);
			if (sym > syM) std::swap(sym, syM);
			if (P.y > syM || P.y < sym || xm > sxM || xM < sxm) {
				continue;
			}
			// find intersection
			float xInt = 0.f;
			// consider collision only if segment is not paraellel to x axis
			if (s1.y - s0.y != 0.f) {
				xInt = s0.x + (P.y - s0.y) * (s1.x - s0.x) / (s1.y - s0.y);
				if (xInt >= xm && xInt <= xM) {
					// collision
					float len = (P.x < xInt ? (xInt - P.x) : (P.x - xInt));
					auto normal = glm::vec3(seg.n, 0.f);
					if (glm::dot(glm::vec3(len, 0.f, 0.f), out.normal) > 0) {
						normal *= -1.f;
					}
					out.update(len, nullptr, normal, 0);
					//out.length = (P.x < xInt ? (xInt - P.x) : (P.x - xInt));
					//out.collide = true;
					//out.normal =
					//out.segmentIndex = 0;
					//return out;
				}
			}
		}
	}
	return out;
}

RayCastHit RayCaster2D::rayCastYGeneric(const glm::vec3 &P, float length, const Shape * s, const glm::mat4 &t) {
	const auto* s2d = static_cast<const Shape2D*>(s);
	auto* segs = s2d->getSegments();
	float ym = P.y, yM = P.y + length;
	if (length < 0) std::swap(ym, yM);
	RayCastHit out;
	if (segs != nullptr) {
		for (auto& seg : *segs) {
			// transform each point
			auto s0 = t * glm::vec4(seg.P0, 0, 1);
			auto s1 = t * glm::vec4(seg.P1, 0, 1);
			float sxm = s0.x, sxM = s1.x;
			float sym = s0.y, syM = s1.y;
			if (sxm > sxM) std::swap(sxm, sxM);
			if (sym > syM) std::swap(sym, syM);
			if (ym > syM || yM < sym || P.x > sxM || P.x < sxm) {
				continue;
			}
			// find intersection
			float yInt = 0.f;
			// consider collision only if segment is not parallel to y axis
			if (s1.x - s0.x != 0.f) {
				yInt = s0.y + (P.x - s0.x) * (s1.y - s0.y) / (s1.x - s0.x);
				if (yInt >= ym && yInt <= yM) {
					// collision
					auto len = (P.y < yInt ? (yInt - P.y) : (P.y - yInt));
					auto normal = glm::vec3(seg.n, 0.f);
					if (glm::dot(glm::vec3(len, 0.f, 0.f), out.normal) > 0) {
						normal *= -1.f;
					}
					out.update(len, nullptr, normal, 0);
					//out.length = (P.y < yInt ? (yInt - P.y) : (P.y - yInt));
					//out.collide = true;
					//out.normal = glm::vec3(seg.n, 0.f);
					//out.segmentIndex = 0;
					//return out;
				}
			}
		}
	}
	return out;
}


RayCastHit RayCaster::raycast(glm::vec3 A, glm::vec3 B, const Shape* s, const glm::mat4& t) {
	auto it = m_functionMap.find(s->get_type_index());
	if (it == m_functionMap.end()) {
		return RayCastHit();
	}
	auto out = it->second(A, B, s, t);
	if (out.collide) {
		// here length is just a number between 0 and 1, so it must be scaled to the proper length
		out.length *= glm::length(B-A);
	}
	return out;
}

RayCastHit RayCaster::raycastX(glm::vec3 P, float length, const Shape * s, const glm::mat4 & t) {
	auto it = m_functionsX.find(s->get_type_index());
	if (it == m_functionsX.end()) {
		return RayCastHit();
	}
	auto out = it->second(P, length, s, t);
	return out;
}

RayCastHit RayCaster::raycastY(glm::vec3 P, float length, const Shape * s, const glm::mat4 & t) {
	auto it = m_functionsY.find(s->get_type_index());
	if (it == m_functionsY.end()) {
		return RayCastHit();
	}
	auto out = it->second(P, length, s, t);
	return out;
}

RayCastHit RayCaster::raycastZ(glm::vec3 P, float length, const Shape * s, const glm::mat4 & t) {
    auto it = m_functionsZ.find(s->get_type_index());
    if (it == m_functionsZ.end()) {
        return RayCastHit();
    }
    auto out = it->second(P, length, s, t);
    return out;
}

void RayCaster2D::updateRaycastHit(RayCastHit& r, glm::vec2 ray, glm::vec2 line, float u, int si) {
	r.collide = true;
	if (u < r.length) {
		r.length = u;
		glm::vec2 d = glm::normalize(line);
		r.normal = glm::vec3(-glm::normalize(ray - d * glm::dot(ray, d)), 0.0f);
		//r.segmentIndex = si;
	}
}

RayCastHit RayCaster2D::rayCastAABB(const glm::vec3 &A, const glm::vec3 &B, const Shape *s, const glm::mat4 &t) {
    auto b = s->getBounds();
    b.translate(t[3]);

    RayCastHit r;
    bool Aout = A.x >= b.max.x || A.x <= b.min.x || A.y >= b.max.y || A.y <= b.min.y;
    bool Bout = B.x >= b.max.x || B.x <= b.min.x || B.y >= b.max.y || B.y <= b.min.y;
    if (Aout == Bout) {
        return r;
    }
    r.collide = true;
    //auto length = glm::length(B-A);
    auto dir = B-A;
    glm::vec2 bx(b.min.x, b.max.x);
    glm::vec2 by(b.min.y, b.max.y);
    float u[4] = { (bx[0] - A.x)/dir.x, (bx[1]-A.x) / dir.x, (by[0]-A.y)/dir.y, (by[1]-A.y)/dir.y};
    glm::vec3 n[2] = {glm::vec3(dir.x > 0 ? -1 : 1, 0.f, 0.f), glm::vec3(0.f, dir.y > 0 ? -1 : 1, 0.f) };
    float umin{2.0f};
    for (size_t i = 0; i < 4; ++i) {
        if (u[i] >= 0 && u[i] <= 1 && u[i] < umin) {
            umin = u[i];
            r.length = umin;
            r.normal = n[i / 2];
        }
    }
    return r;

}

RayCastHit RayCaster2D::rayCastPoly(const glm::vec3& A, const glm::vec3& B, const Shape *s, const glm::mat4 &t) {
	RayCastHit out;
	float u {};
	auto ray = glm::vec2(B - A);
	const auto* poly = static_cast<const ConvexPoly*>(s);
	const auto& vertices = poly->getPoints();
	glm::vec2 previous = t * glm::vec4(vertices[0], 0.f, 1.f);
	glm::vec2 curr(0.f);
	size_t n = vertices.size();
	for (size_t i = 1; i <= vertices.size(); ++i) {
		curr = t * glm::vec4(vertices[i % n], 0.0f, 1.0f);
		if (seg2seg (A, B, previous, curr, u)) {
			// update raycast hit
			updateRaycastHit(out, ray, curr-previous, u, i);
		}
		previous=curr;
	}




	return out;
}
