#include "intersector2d.h"
#include "../util.h"
#include "../shapes/convexpoly.h"
#include "../shapes/circle.h"
#include "../shapes/compound.h"
#include "../shapes/aabb.h"
#include "polygon.h"
#include "../math/geometry.h"

using namespace shapes;


Intersector2D::Intersector2D() {
    add(ShapeType::CONVEX_POLY, ShapeType::CONVEX_POLY, geometry::SAT_convex);
    add(ShapeType::AABB, ShapeType::CONVEX_POLY, geometry::SAT_AABB_convex);
    add(ShapeType::CIRCLE, ShapeType::CONVEX_POLY, geometry::SAT_circle_convex);
	add(ShapeType::AABB, ShapeType::AABB, geometry::SAT_AABB_convex);



//    add<ConvexPoly, ConvexPoly>([&] (const Shape* s1, const Shape* s2, const glm::mat4& t1, const glm::mat4& t2) { return SAT(s1, s2, t1, t2); });
//    add<Rect, ConvexPoly>([&] (const Shape* s1, const Shape* s2, const glm::mat4& t1, const glm::mat4& t2) { return SAT(s1, s2, t1, t2); });
//    add<Segment, ConvexPoly>([&] (const Shape* s1, const Shape* s2, const glm::mat4& t1, const glm::mat4& t2) { return SAT(s1, s2, t1, t2); });
//    add<Rect, Rect>([&] (const Shape* s1, const Shape* s2, const glm::mat4& t1, const glm::mat4& t2) { return SAT(s1, s2, t1, t2); });
//    add<Rect, Segment>([&] (const Shape* s1, const Shape* s2, const glm::mat4& t1, const glm::mat4& t2) { return SAT(s1, s2, t1, t2); });
//    add<Segment, Segment>([&] (const Shape* s1, const Shape* s2, const glm::mat4& t1, const glm::mat4& t2) { return SAT(s1, s2, t1, t2); });
//
//    // circle
//    add<ConvexPoly, Circle>([&] (const Shape* s1, const Shape* s2, const glm::mat4& t1, const glm::mat4& t2) { return SATCircle(s1, s2, t1, t2); });
//    add<Rect, Circle>([&] (const Shape* s1, const Shape* s2, const glm::mat4& t1, const glm::mat4& t2) { return SATCircle(s1, s2, t1, t2); });
//    add<Segment, Circle>([&] (const Shape* s1, const Shape* s2, const glm::mat4& t1, const glm::mat4& t2) { return SATCircle(s1, s2, t1, t2); });
//
//    //add<CompoundShape, ConvexPoly> ([&] (const Shape* s1, const Shape* s2, const glm::mat4& t1, const glm::mat4& t2) { return compound(s1, s2, t1, t2); });
//    //add<CompoundShape, Rect> ([&] (const Shape* s1, const Shape* s2, const glm::mat4& t1, const glm::mat4& t2) { return compound(s1, s2, t1, t2); });
//    //add<CompoundShape, Segment> ([&] (const Shape* s1, const Shape* s2, const glm::mat4& t1, const glm::mat4& t2) { return compound(s1, s2, t1, t2); });
//    //add<CompoundShape, Circle> ([&] (const Shape* s1, const Shape* s2, const glm::mat4& t1, const glm::mat4& t2) { return compound(s1, s2, t1, t2); });
//    //add<CompoundShape, AABB> ([&] (const Shape* s1, const Shape* s2, const glm::mat4& t1, const glm::mat4& t2) { return compound(s1, s2, t1, t2); });
//
//    add<AABB, ConvexPoly>([&] (const Shape* s1, const Shape* s2, const glm::mat4& t1, const glm::mat4& t2) { return SATAABB(s1, s2, t1, t2); });
//    add<AABB, Rect>([&] (const Shape* s1, const Shape* s2, const glm::mat4& t1, const glm::mat4& t2) { return SATAABB(s1, s2, t1, t2); });
//    add<AABB, Segment>([&] (const Shape* s1, const Shape* s2, const glm::mat4& t1, const glm::mat4& t2) { return SATAABB(s1, s2, t1, t2); });
//    add<AABB, AABB>([&] (const Shape* s1, const Shape* s2, const glm::mat4& t1, const glm::mat4& t2) { return AABB2(s1, s2, t1, t2); });
//	add<AABB, Polygon>([&] (const Shape* s1, const Shape* s2, const glm::mat4& t1, const glm::mat4& t2) { return AABBvsPolygon(s1, s2, t1, t2); });
//
//    //add<AABB, Triangles>([&] (const Shape* s1, const Shape* s2, const glm::mat4& t1, const glm::mat4& t2) { return SATTriAABB(s1, s2, t1, t2); });
//    //add<Point, AABB>([&] (const Shape* s1, const Shape* s2, const glm::mat4& t1, const glm::mat4& t2) { return PointVsShape(s1, s2, t1, t2); });
//	//add<Point, Polygon>([&] (const Shape* s1, const Shape* s2, const glm::mat4& t1, const glm::mat4& t2) { return PointVsShape(s1, s2, t1, t2); });

}





CollisionReport Intersector2D::AABBvsPolygon(const Shape * aabb, const Shape * shape, const glm::mat4 & t1, const glm::mat4& t2) {
	auto center = glm::vec3(t1[3]) + aabb->getBounds().getCenter();
	glm::vec3 pp = glm::inverse(t2) * glm::vec4(center, 1.f);
	CollisionReport report;
	report.collide = shape->isInside(pp);
	return report;



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
//
//CollisionReport Intersector2D::compound(const Shape * s1, const Shape * s2, const glm::mat4 & t1, const glm::mat4 & t2) {
//	const auto* cs = static_cast<const CompoundShape*>(s1);
//	for (const auto& s : cs->getShapes()) {
//		auto report = this->intersect(s.get(), s2, t1, t2);
//		if (report.collide) {
//			return report;
//		}
//	}
//	return CollisionReport();
//}

