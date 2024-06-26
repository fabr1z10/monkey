#pragma once

#include "bounds.h"
#include <typeindex>
#include <vector>

enum class ShapeType {
    RECT, COMPOUND, AABB2D, POLYLINE, AABB3D, POLYGON
};

struct Seg {
	glm::vec2 P0;
	glm::vec2 P1;
	glm::vec2 n;
};

struct Face {
	glm::vec3 A, B, C;
	bool quad;
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

class Point : public Shape {
public:
	Point() : Shape() {}


};




class Shape2D : public Shape {
public:
    // project shape onto axis
    virtual const std::vector<Seg>* getSegments() const { return nullptr; }

};



class Shape3D : public Shape {
public:
	// project shape onto axis
	virtual const std::vector<Face>* getFaces() { return nullptr; }
};


class ConvexShape : public Shape2D {
public:
	virtual glm::vec2 project(glm::vec2, const glm::mat4&) const = 0;
};





