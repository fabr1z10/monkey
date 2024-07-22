#pragma once

#include "../shape.h"
#include <pybind11/pybind11.h>



class PolyLine : public Shape2D {
public:
	PolyLine(const std::vector<float>& points);
	//PolyLine(const std::vector<glm::vec2>& pts);
	const std::vector<Seg>* getSegments() const override;
	const std::vector<glm::vec2>& getPoints() const;
	float getY(float x) const;
private:
    void addSeg(glm::vec2 A, glm::vec2 B);
    std::vector<glm::vec2> _points;
    std::vector<Seg> _segs;
};

inline const std::vector<Seg>* PolyLine::getSegments() const {
	return &_segs;
}

inline const std::vector<glm::vec2> & PolyLine::getPoints() const {
	return _points;
}
