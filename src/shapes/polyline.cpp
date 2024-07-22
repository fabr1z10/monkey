#include "polyline.h"
#include "../pyhelper.h"

PolyLine::PolyLine(const std::vector<float>& pts) {

	//auto pts = py_get_dict<std::vector<float>>(args, "points");
	assert(pts.size() >= 4);
	glm::vec2 previous(pts[0], pts[1]);
	_points.push_back(previous);
	m_bounds.addPoint(previous);
	for (size_t i = 2; i < pts.size(); i+=2) {
		Seg s;
		s.P0 =  previous;
		s.P1 = {pts[i], pts[i+1]};
		_points.push_back(s.P1);
		m_bounds.addPoint(s.P1);
		glm::vec2 v = s.P1 - s.P0;
		s.n = glm::normalize(glm::vec2(-v.y, v.x));
		_segs.push_back(s);
		previous = s.P1;
	}
	m_type = ShapeType::POLYLINE;

}

void PolyLine::addSeg(glm::vec2 A, glm::vec2 B) {
    Seg s;
    s.P0 = A;
    s.P1 = B;
    m_bounds.addPoint(B);
    glm::vec2 v = s.P1 - s.P0;
    s.n = glm::normalize(glm::vec2(-v.y, v.x));
    _segs.push_back(s);
}
//
//PolyLine::PolyLine(const std::vector<glm::vec2> &pts) : _points(pts) {
//    for (size_t i = 0; i < _points.size(); ++i) {
//        addSeg(_points[i], _points[(i+1) % _points.size()]);
//    }
//    m_type = ShapeType::POLYLINE;
//
//}

float PolyLine::getY(float x) const {
	int i = 0;
	// find 1st item strictly greater than x
	while (i < _points.size() && _points[i].x <= x) i++;
	if (i == 0) {
		return _points[0].y;
	} else if (i == _points.size()) {
		return _points.back().y;
	}
	return _points[i-1].y + (_points[i].y - _points[i-1].y) * (x - _points[i-1].x) / (_points[i].x - _points[i-1].x);



}