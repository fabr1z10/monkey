#include "intersector2d.h"
#include "../util.h"
#include "../shapes/convexpoly.h"
#include "../shapes/circle.h"
#include "../shapes/compound.h"
#include "../shapes/aabb.h"
#include "polygon.h"

Intersector2D::Intersector2D() {

    add<ConvexPoly, ConvexPoly>([&] (const Shape* s1, const Shape* s2, const glm::mat4& t1, const glm::mat4& t2) { return SAT(s1, s2, t1, t2); });
    add<Rect, ConvexPoly>([&] (const Shape* s1, const Shape* s2, const glm::mat4& t1, const glm::mat4& t2) { return SAT(s1, s2, t1, t2); });
    add<Segment, ConvexPoly>([&] (const Shape* s1, const Shape* s2, const glm::mat4& t1, const glm::mat4& t2) { return SAT(s1, s2, t1, t2); });
    add<Rect, Rect>([&] (const Shape* s1, const Shape* s2, const glm::mat4& t1, const glm::mat4& t2) { return SAT(s1, s2, t1, t2); });
    add<Rect, Segment>([&] (const Shape* s1, const Shape* s2, const glm::mat4& t1, const glm::mat4& t2) { return SAT(s1, s2, t1, t2); });
    add<Segment, Segment>([&] (const Shape* s1, const Shape* s2, const glm::mat4& t1, const glm::mat4& t2) { return SAT(s1, s2, t1, t2); });

    // circle
    add<ConvexPoly, Circle>([&] (const Shape* s1, const Shape* s2, const glm::mat4& t1, const glm::mat4& t2) { return SATCircle(s1, s2, t1, t2); });
    add<Rect, Circle>([&] (const Shape* s1, const Shape* s2, const glm::mat4& t1, const glm::mat4& t2) { return SATCircle(s1, s2, t1, t2); });
    add<Segment, Circle>([&] (const Shape* s1, const Shape* s2, const glm::mat4& t1, const glm::mat4& t2) { return SATCircle(s1, s2, t1, t2); });

    add<CompoundShape, ConvexPoly> ([&] (const Shape* s1, const Shape* s2, const glm::mat4& t1, const glm::mat4& t2) { return compound(s1, s2, t1, t2); });
    add<CompoundShape, Rect> ([&] (const Shape* s1, const Shape* s2, const glm::mat4& t1, const glm::mat4& t2) { return compound(s1, s2, t1, t2); });
    add<CompoundShape, Segment> ([&] (const Shape* s1, const Shape* s2, const glm::mat4& t1, const glm::mat4& t2) { return compound(s1, s2, t1, t2); });
    add<CompoundShape, Circle> ([&] (const Shape* s1, const Shape* s2, const glm::mat4& t1, const glm::mat4& t2) { return compound(s1, s2, t1, t2); });
    add<CompoundShape, AABB> ([&] (const Shape* s1, const Shape* s2, const glm::mat4& t1, const glm::mat4& t2) { return compound(s1, s2, t1, t2); });

    add<AABB, ConvexPoly>([&] (const Shape* s1, const Shape* s2, const glm::mat4& t1, const glm::mat4& t2) { return SATAABB(s1, s2, t1, t2); });
    add<AABB, Rect>([&] (const Shape* s1, const Shape* s2, const glm::mat4& t1, const glm::mat4& t2) { return SATAABB(s1, s2, t1, t2); });
    add<AABB, Segment>([&] (const Shape* s1, const Shape* s2, const glm::mat4& t1, const glm::mat4& t2) { return SATAABB(s1, s2, t1, t2); });
    add<AABB, AABB>([&] (const Shape* s1, const Shape* s2, const glm::mat4& t1, const glm::mat4& t2) { return AABB2(s1, s2, t1, t2); });
    //add<AABB, Triangles>([&] (const Shape* s1, const Shape* s2, const glm::mat4& t1, const glm::mat4& t2) { return SATTriAABB(s1, s2, t1, t2); });
    add<Point, AABB>([&] (const Shape* s1, const Shape* s2, const glm::mat4& t1, const glm::mat4& t2) { return PointVsShape(s1, s2, t1, t2); });
	add<Point, Polygon>([&] (const Shape* s1, const Shape* s2, const glm::mat4& t1, const glm::mat4& t2) { return PointVsShape(s1, s2, t1, t2); });

}

CollisionReport Intersector2D::SAT(const Shape * s1, const Shape * s2, const glm::mat4 & t1, const glm::mat4 & t2) {
    const auto* cp1 = static_cast<const ConvexPoly*>(s1);
    const auto* cp2 = static_cast<const ConvexPoly*>(s2);
    std::vector<glm::vec2> axes;
    for (const auto& t : cp1->getUnitNormals()) axes.push_back(t1 * glm::vec4(t, 0.f, 0.f));
    for (const auto& t : cp2->getUnitNormals()) axes.push_back(t2 * glm::vec4(t, 0.f, 0.f));
    //axes.insert(axes.end(), cp2->getUnitNormals().begin(), cp2->getUnitNormals().end());
    return performSAT(axes, cp1, cp2, t1, t2);
}

CollisionReport Intersector2D::performSAT(const std::vector<glm::vec2>& axes, const ConvexShape* a, const ConvexShape* b, const glm::mat4& t1, const glm::mat4& t2) {
    // axes are given in world coordinates
    // since dot products do not depend on coordinate systems, we transform axes from world to local
    CollisionReport report;
    report.distance = std::numeric_limits<float>::infinity();
    for (auto& axis : axes) {
        glm::vec2 pA = a->project(axis, t1);
        glm::vec2 pB = b->project(axis, t2);
        if (pA.x > pB.y || pB.x > pA.y) {
            // we found a non-overlapping axis
            // so we are sure that the two shapes do not collide
            report.collide = false;
            return report;
        }
        // there's overlapping in this axis
        // compute overlap
        float overlap = computeOverlap(pA, pB);
        if (fabs(overlap) < fabs(report.distance)) {
            report.distance = fabs(overlap);
            report.direction = glm::vec3(axis * (overlap < 0 ? -1.0f : 1.0f), 0.f);
        }
    }
    // if we get here, it means we haven't found a separating axis.
    // Therefore, we can conclude the two shapes collide
    report.collide = true;
    return report;
}

CollisionReport Intersector2D::SATCircle(const Shape * s1, const Shape * s2, const glm::mat4 & t1, const glm::mat4 & t2) {
    const auto* cp1 = static_cast<const ConvexPoly*>(s1);
    const auto* sh2 = static_cast<const ConvexShape*>(s2);

    // get circle center in world coordinates
    glm::vec2 centerWorld = t2 * glm::vec4(s2->getOffset(), 1.0f);
    float mind2 = std::numeric_limits<float>::infinity();
    glm::vec2 circleAxis(0.0f);
    for (const auto& p : cp1->getPoints()) {
        glm::vec2 pw = t1 * glm::vec4(p, 0.0f, 1.0f);
        auto d = centerWorld - pw;
        float d2 = d.x * d.x + d.y * d.y;
        if (d2 <= mind2) {
            mind2 = d2;
            circleAxis = d;
        }
    }
    std::vector<glm::vec2> axes;
    axes.push_back(glm::normalize(circleAxis));

    for (const auto& t : cp1->getUnitNormals()) axes.push_back(t1 * glm::vec4(t, 0.f, 0.f));
    return performSAT(axes, cp1, sh2, t1, t2);

}

CollisionReport Intersector2D::PointVsShape(const Shape * point, const Shape * shape, const glm::mat4 & pointTransform, const glm::mat4 & shapeTransform) {
	// point pos is now in world coords. need to convert it to shape coords
	glm::vec3 pointPos = pointTransform[3];

	glm::vec3 pp = glm::inverse(shapeTransform) * glm::vec4(pointPos, 1.f);
	CollisionReport report;
	report.collide = shape->isInside(pp);
	return report;

}


CollisionReport Intersector2D::AABB2(const Shape * s1, const Shape * s2, const glm::mat4 & t1, const glm::mat4 & t2) {
	auto b1 = s1->getBounds();
	auto b2 = s2->getBounds();

	b1.transform(t1);
	b2.transform(t2);

	CollisionReport report;
	report.collide = !(b1.min.x > b2.max.x || b2.min.x > b1.max.x || b1.min.y > b2.max.y || b2.min.y > b1.max.y);
	return report;

}

CollisionReport Intersector2D::compound(const Shape * s1, const Shape * s2, const glm::mat4 & t1, const glm::mat4 & t2) {
	const auto* cs = static_cast<const CompoundShape*>(s1);
	for (const auto& s : cs->getShapes()) {
		auto report = this->intersect(s.get(), s2, t1, t2);
		if (report.collide) {
			return report;
		}
	}
	return CollisionReport();
}

CollisionReport Intersector2D::SATAABB(const Shape * s1, const Shape * s2, const glm::mat4 & t1, const glm::mat4 & t2) {
	const auto* cp1 = static_cast<const ConvexShape*>(s1);
	const auto* cp2 = static_cast<const ConvexPoly*>(s2);
	std::vector<glm::vec2> axes { glm::vec2(1.f, 0.f), glm::vec2(0.f, 1.f)};
	for (const auto& t : cp2->getUnitNormals()) axes.push_back(t2 * glm::vec4(t, 0.f, 0.f));
	//axes.insert(axes.end(), cp2->getUnitNormals().begin(), cp2->getUnitNormals().end());
	return performSAT(axes, cp1, cp2, t1, t2);
}