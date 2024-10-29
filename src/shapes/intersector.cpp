#include "intersector.h"

using namespace shapes;

void Intersector::add(shapes::ShapeType t1, shapes::ShapeType t2, methodPtr ptr) {
    _map[{t1, t2}] = ptr;
}


CollisionReport Intersector::intersect(shapes::Shape * s1, shapes::Shape * s2, const glm::mat4 & t1, const glm::mat4 & t2) {
    if (s1 == nullptr || s2 == nullptr) {
        return CollisionReport();
    }
    auto it = _map.find(std::make_pair(s1->getShapeType(), s2->getShapeType()));
    if (it == _map.end()) {
        auto it2 = _map.find(std::make_pair(s2->getShapeType(), s1->getShapeType()));
        if (it2 == _map.end()) {
            return CollisionReport();
        }
        auto report = it2->second(s2, s1, t2, t1);
        if (report.collide) {
            report.direction *= -1.f;
        }
        return report;
    } else {
        return it->second(s1, s2, t1, t2);
    }
}





