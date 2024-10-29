#pragma once

#include "../shapes/convexpoly.h"

struct CollisionReport {
    CollisionReport() : collide(false) {}

    // tells whether a collision has been found
    bool collide;
    float distance;
    glm::vec3 direction;
};

namespace geometry {

    /* Collection of methods to run collision checks*/
    CollisionReport SAT_convex(shapes::Shape *, shapes::Shape *, const glm::mat4 &, const glm::mat4 &);
    CollisionReport SAT_AABB_convex(shapes::Shape *, shapes::Shape *, const glm::mat4 &, const glm::mat4 &);
    CollisionReport SAT_circle_convex(shapes::Shape *, shapes::Shape *, const glm::mat4 &, const glm::mat4 &);




    CollisionReport SAT(const std::vector<glm::vec2>& axes,
        const shapes::ConvexShape* a, const shapes::ConvexShape* b, const glm::mat4& t1, const glm::mat4& t2);


}