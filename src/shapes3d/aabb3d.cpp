#include "aabb3d.h"

AABB3D::AABB3D(float xmin, float xmax, float ymin, float ymax, float zmin, float zmax) :
		_xMin(xmin), _xMax(xmax), _yMin(ymin), _yMax(ymax), _zMin(zmin), _zMax(zmax) {
    m_type = ShapeType::AABB3D;
	m_bounds.min = glm::vec3(xmin, ymin, zmin);
	m_bounds.max = glm::vec3(xmax, ymax, zmax);
}