#include "raycaster.h"
#include "../shapes/convexpoly.h"
#include "../shapes/aabb.h"
#include "../util.h"

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

};

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
