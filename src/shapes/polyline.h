#pragma once

#include "../shape.h"
#include <pybind11/pybind11.h>



class PolyLine : public Shape2D {
public:
	PolyLine(const pybind11::kwargs& args);
	const std::vector<Seg>* getSegments() const override;
	const std::vector<glm::vec2>& getPoints() const;
	float getY(float x) const;
private:
    std::vector<glm::vec2> _points;
    std::vector<Seg> _segs;
};

inline const std::vector<Seg>* PolyLine::getSegments() const {
	return &_segs;
}

inline const std::vector<glm::vec2> & PolyLine::getPoints() const {
	return _points;
}
