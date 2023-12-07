#pragma once


#include "intersector.h"
#include "../runners/collision.h"


class Intersector2D : public Intersector {
public:
    Intersector2D();

private:
    CollisionReport performSAT(const std::vector<glm::vec2>& axesw, const ConvexShape* a, const ConvexShape* b, const glm::mat4& t1, const glm::mat4& t2);
    //CollisionReport SATTriAABB(const Shape*, const Shape*, const glm::mat4&, const glm::mat4&);
    CollisionReport SAT(const Shape*, const Shape*, const glm::mat4&, const glm::mat4&);
    CollisionReport SATAABB(const Shape*, const Shape*, const glm::mat4&, const glm::mat4&);
    CollisionReport SATCircle(const Shape *, const Shape*, const glm::mat4&, const glm::mat4& );
    CollisionReport compound(const Shape *, const Shape*, const glm::mat4&, const glm::mat4& );
    CollisionReport AABB2(const Shape*, const Shape*, const glm::mat4&, const glm::mat4&);
    CollisionReport PointVsShape(const Shape*, const Shape*, const glm::mat4&, const glm::mat4&);

};

