#pragma once

#include "../shape.h"
#include <pybind11/pybind11.h>

namespace shapes {

    class PolyLine : public Shape {
    public:
        /* This is a polygonal chain i.e. a connected series of line segments */
        explicit PolyLine(const std::vector<float> &points);

        //PolyLine(const std::vector<glm::vec2>& pts);
        const std::vector<Seg> *getSegments() const override;

        const std::vector<glm::vec2> &getPoints() const;

        float getY(float x) const;

        RaycastResult raycast(glm::vec2 P0, glm::vec2 P1) const override;

        bool isInside(glm::vec2) const override;

    private:
        void addSeg(glm::vec2 A, glm::vec2 B);

        std::vector<glm::vec2> _points;
        std::vector<Seg> _segs;
    };

    inline const std::vector<Seg> *PolyLine::getSegments() const {
        return &_segs;
    }

    inline const std::vector<glm::vec2> &PolyLine::getPoints() const {
        return _points;
    }

}