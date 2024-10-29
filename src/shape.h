#pragma once

#include "bounds.h"
#include <typeindex>
#include <vector>


namespace shapes {

    enum class ShapeType {
        AABB,
        CONVEX_POLY,
        CIRCLE,
        POLYGON,
        POLYLINE


    };

    struct RaycastResult {
        RaycastResult() : collide(false), length(0.f), normal(glm::vec2(0.f)) {}

        RaycastResult(bool c, float length, glm::vec2 n) : collide(c), length(length), normal(n) {}
        bool collide;
        float length;
        glm::vec2 normal;
    };

    struct Seg {
        glm::vec2 P0;
        glm::vec2 P1;
        glm::vec2 n;
    };

    enum Direction { X, Y };


    class Shape {
    public:
        Shape() = default;

        virtual std::type_index get_type_index() const {
            return std::type_index(typeid(*this));
        }

        //glm::vec2 getOffset() const;

        //void setOffset(float x, float y);

        Bounds getBounds() const;

        ShapeType getShapeType() const;

        virtual bool isInside(glm::vec2) const = 0;

        // performs a raycast - P0 and P1 should be provided in local coordinates
        virtual RaycastResult raycast(glm::vec2 P0, glm::vec2 P1) const = 0;

        virtual const std::vector<Seg> *getSegments() const { return nullptr; }

    protected:
        bool rayOutsideBounds(glm::vec2 P0, glm::vec2 P1) const;

        Bounds m_bounds;
        //glm::vec2 m_offset;
        ShapeType m_type;
    };

//    inline glm::vec2 Shape::getOffset() const {
//        return m_offset;
//    }
//
//    inline void Shape::setOffset(float x, float y) {
//        m_offset = glm::vec2(x, y);
//    }

    inline ShapeType Shape::getShapeType() const {
        return m_type;
    }

    inline Bounds Shape::getBounds() const {
        return m_bounds;
    }


    class ConvexShape : public Shape {
    public:
        virtual glm::vec2 project(glm::vec2, const glm::mat4 &) const = 0;
    };


}


