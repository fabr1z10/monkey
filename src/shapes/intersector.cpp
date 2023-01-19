#include "intersector.h"

CollisionReport Intersector::intersect(const Shape * s1, const Shape * s2, const glm::mat4 & t1, const glm::mat4 & t2) {
	if (s1 == nullptr || s2 == nullptr) {
		return CollisionReport();
	}
	auto it = m_functionMap.find(std::make_pair(s1->get_type_index(), s2->get_type_index()));
	if (it == m_functionMap.end()) {
		auto it2 = m_functionMap.find(std::make_pair(s2->get_type_index(), s1->get_type_index()));
		if (it2 == m_functionMap.end()) {
			return CollisionReport();
		}
		auto report = it2->second(s2, s1, t2, t1);
		if (report.collide) {
			report.direction *= -1.f;
		}
		return report;
	}

	return it->second(s1, s2, t1, t2);
}