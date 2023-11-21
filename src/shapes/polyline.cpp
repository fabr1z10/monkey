#include "polyline.h"
#include "../pyhelper.h"

PolyLine::PolyLine(const pybind11::kwargs &args) {

	auto pts = py_get_dict<std::vector<float>>(args, "points");
	assert(pts.size() >= 4);
	glm::vec2 previous(pts[0], pts[1]);
	m_bounds.addPoint(previous);
	for (size_t i = 2; i < pts.size(); i+=2) {
		Seg s;
		s.P0 =  previous;
		s.P1 = {pts[i], pts[i+1]};
		m_bounds.addPoint(s.P1);
		glm::vec2 v = s.P1 - s.P0;
		s.n = glm::normalize(glm::vec2(-v.y, v.x));
		_segs.push_back(s);
		previous = s.P1;
	}
	m_type = ShapeType::POLYLINE;

}

