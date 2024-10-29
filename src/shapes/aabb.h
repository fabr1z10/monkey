#pragma once

// this special class is just a box of fixed size that cannot rotate.
// this is ok for handling collisions in platformer or other games where
// the nodes can only translate but never rotate!

#include "../shape.h"
#include <array>

namespace shapes {

    class AABB : public ConvexShape {
    public:
        AABB(float xmin, float xmax, float ymin, float ymax);

        glm::vec2 project(glm::vec2, const glm::mat4 &) const override;

        bool isInside(glm::vec2 P) const override;

        glm::vec2 center;
        float halfDiag;

        const std::array<glm::vec2, 4> &getPoints() const;

        RaycastResult raycast(glm::vec2 P0, glm::vec2 P1) const override;

    private:
        // test collision with a vertical side at x
        bool test_x(glm::vec2 P0, glm::vec2 P1, float x, float& u) const;

        // test collision with a horizontal side at y
        bool test_y(glm::vec2 P0, glm::vec2 P1, float y, float& u) const;

        float _xMin;
        float _xMax;
        float _yMin;
        float _yMax;
        std::array<glm::vec2, 4> m_points;
    };

    inline const std::array<glm::vec2, 4> &AABB::getPoints() const {
        return m_points;
    }
}