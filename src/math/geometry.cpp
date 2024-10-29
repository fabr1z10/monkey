#include "geometry.h"
#include "../shapes/circle.h"
#include "../util.h"

using namespace geometry;
using namespace shapes;

CollisionReport geometry::SAT_convex(Shape * s1, Shape * s2, const glm::mat4 & t1, const glm::mat4 & t2) {
    const auto* cp1 = static_cast<const ConvexPoly*>(s1);
    const auto* cp2 = static_cast<const ConvexPoly*>(s2);

    const auto& n1 = cp1->getUnitNormals();
    const auto& n2 = cp1->getUnitNormals();

    std::vector<glm::vec2> axes(n1.size() + n2.size());

    // Append squared values of vec1 to vec2
    size_t i {0};
    for (const auto& n : n1) {
        axes[i++] = t1 * glm::vec4(n, 0.f, 0.f);
    }
    for (const auto& n : n2) {
        axes[i++] = t2 * glm::vec4(n, 0.f, 0.f);
    }

    return SAT(axes, cp1, cp2, t1, t2);
}

CollisionReport geometry::SAT_AABB_convex(Shape * s1, Shape * s2, const glm::mat4 & t1, const glm::mat4 & t2) {
    const auto* aabb = static_cast<const ConvexShape*>(s1);
    const auto* convex = static_cast<const ConvexPoly*>(s2);

    const auto& n2 = convex->getUnitNormals();

    std::vector<glm::vec2> axes(2 + n2.size());

    axes[0] = glm::vec2(1.f, 0.f);
    axes[1] = glm::vec2(0.f, 1.f);

    size_t i{2};
    for (const auto& n : n2) {
        axes[i++] = t2 * glm::vec4(n, 0.f, 0.f);
    }

    return SAT(axes, aabb, convex, t1, t2);
}

CollisionReport geometry::SAT_circle_convex(Shape * s1, Shape * s2, const glm::mat4 & t1, const glm::mat4 & t2) {
    const auto* circle = static_cast<const Circle*>(s1);
    const auto* convex = static_cast<const ConvexPoly*>(s2);

    const auto& normals = convex->getUnitNormals();
    std::vector<glm::vec2> axes(1 + normals.size());

    // get circle center in world coordinates
    glm::vec2 centerWorld = t1 * glm::vec4(circle->getCenter(), 0.f, 1.0f);
    float mind2 = std::numeric_limits<float>::infinity();
    glm::vec2 circleAxis(0.0f);
    for (const auto& p : convex->getPoints()) {
        glm::vec2 pw = t2 * glm::vec4(p, 0.0f, 1.0f);
        auto d = centerWorld - pw;
        float d2 = d.x * d.x + d.y * d.y;
        if (d2 <= mind2) {
            mind2 = d2;
            circleAxis = d;
        }
    }

    axes[0] = glm::normalize(circleAxis);

    size_t i{1};
    for (const auto& n : normals) {
        axes[i++] = t2 * glm::vec4(n, 0.f, 0.f);
    }
    return SAT(axes, circle, convex, t1, t2);

}




CollisionReport geometry::SAT(const std::vector<glm::vec2>& axes, const ConvexShape* a, const ConvexShape* b, const glm::mat4& t1, const glm::mat4& t2) {
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