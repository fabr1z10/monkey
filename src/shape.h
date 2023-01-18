#pragma once

#include "bounds.h"
#include <typeindex>

enum class ShapeType {
    RECT
};


class Shape {
public:
    Shape() = default;
    virtual std::type_index get_type_index( ) const
    {
        return std::type_index( typeid(*this) );
    }
    glm::vec3 getOffset() const;
    void setOffset(float x, float y, float z);
    Bounds getBounds() const;
    ShapeType getShapeType() const;
    virtual bool isInside(glm::vec3) const;
protected:
    Bounds m_bounds;
    glm::vec3 m_offset;
    ShapeType m_type;
};

inline bool Shape::isInside(glm::vec3) const {
    return false;
}


inline glm::vec3 Shape::getOffset() const {
    return m_offset;
}

inline void Shape::setOffset(float x, float y, float z) {
    m_offset = glm::vec3(x, y, z);
}

inline ShapeType Shape::getShapeType() const {
    return m_type;
}

inline Bounds Shape::getBounds() const {
    return m_bounds;
}


class Shape2D : public Shape {
public:
    // project shape onto axis
    virtual glm::vec2 project(glm::vec2, const glm::mat4&) const = 0;
};


