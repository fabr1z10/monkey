#pragma once


#include "intersector.h"
#include "../runners/collision.h"


class Intersector2D : public Intersector {
public:
    Intersector2D();

private:
    CollisionReport performSAT(const std::vector<glm::vec2>& axesw, const shapes::ConvexShape* a, const shapes::ConvexShape* b, const glm::mat4& t1, const glm::mat4& t2);
    //CollisionReport SATTriAABB(const Shape*, const Shape*, const glm::mat4&, const glm::mat4&);
    CollisionReport SAT(const shapes::Shape*, const shapes::Shape*, const glm::mat4&, const glm::mat4&);
    CollisionReport SATAABB(const shapes::Shape*, const shapes::Shape*, const glm::mat4&, const glm::mat4&);
    CollisionReport SATCircle(const shapes::Shape *, const shapes::Shape*, const glm::mat4&, const glm::mat4& );
    CollisionReport compound(const shapes::Shape *, const shapes::Shape*, const glm::mat4&, const glm::mat4& );
    CollisionReport AABB2(const shapes::Shape*, const shapes::Shape*, const glm::mat4&, const glm::mat4&);
    CollisionReport PointVsShape(const shapes::Shape*, const shapes::Shape*, const glm::mat4&, const glm::mat4&);
    CollisionReport AABBvsPolygon(const shapes::Shape*, const shapes::Shape*, const glm::mat4&, const glm::mat4&);

};

