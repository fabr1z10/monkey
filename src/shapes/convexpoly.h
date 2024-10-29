#pragma once

#include "../shape.h"
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <vector>

namespace py = pybind11;

namespace shapes {
    class ConvexPoly : public ConvexShape {
    public:
        ConvexPoly();

        explicit ConvexPoly(const std::vector<float>& input);

        glm::vec2 project(glm::vec2, const glm::mat4 &) const override;

        const std::vector<glm::vec2> &getPoints() const;

        const std::vector<glm::vec2> &getUnitNormals() const;

        const std::vector<Seg> *getSegments() const override;

        RaycastResult raycast(glm::vec2 P0, glm::vec2 P1) const override;

        bool isInside(glm::vec2) const override;

    protected:
        // add point may call addedge which in turn adds the segment, the normal and update bounds
        void addPoint(glm::vec2);

        void addEdge(glm::vec2 &A, glm::vec2 &B);

        void closeLoop();

        std::vector<glm::vec2> m_points;
        std::vector<Seg> _segments;
        //std::vector<glm::vec2> m_edges;
        // unit normals
        std::vector<glm::vec2> m_normals;

    };

    inline const std::vector<Seg> *ConvexPoly::getSegments() const {
        return &_segments;
    }


    class Rect : public ConvexPoly {
    public:
        Rect(float w, float h, glm::vec2 anchorPoint = glm::vec2(0.f));

    };


    class Segment : public ConvexPoly {
    public:
        Segment(float x0, float y0, float x1, float y1);

        bool isInside(glm::vec2) const override;

    };

    inline bool Segment::isInside(glm::vec2) const {
        return false;

    }


}