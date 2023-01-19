#include "seg3d.h"

Segment3D::Segment3D(glm::vec3 A, glm::vec3 B) : Shape(), m_A(A), m_B(B) {
	//m_type = ShapeType::SEGMENT3D;
	m_bounds.min.x = std::min(A.x, B.x);
	m_bounds.min.y = std::min(A.y, B.y);
	m_bounds.min.z = std::min(A.z, B.z);
	m_bounds.max.x = std::max(A.x, B.x);
	m_bounds.max.y = std::max(A.y, B.y);
	m_bounds.max.z = std::max(A.z, B.z);
}