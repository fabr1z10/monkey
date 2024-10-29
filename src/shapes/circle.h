#pragma once

#include "../shape.h"
#include <vector>

namespace shapes {

    class Circle : public ConvexShape {
    public:
        explicit Circle(float radius, glm::vec2 center = glm::vec2(0.f));

        glm::vec2 project(glm::vec2, const glm::mat4 &) const override;

        glm::vec2 getCenter() const;

        float getRadius() const;

        bool isInside(glm::vec2) const override;

        RaycastResult raycast(glm::vec2 P0, glm::vec2 P1) const override;

    protected:
        glm::vec2 _center;
        float m_radius;
        float _r2;

    };

    inline float Circle::getRadius() const {
        return m_radius;
    }

    inline glm::vec2 Circle::getCenter() const {
        return _center;
    }
}
