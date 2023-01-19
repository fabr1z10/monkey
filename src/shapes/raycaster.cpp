#include "raycaster.h"
#include "../shapes/convexpoly.h"
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
