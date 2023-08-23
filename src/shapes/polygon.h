#pragma once

#include "../shape.h"
#include <vector>


// generic polygon, can be concave
// can have holes

class Polygon : public Shape {
public:
	Polygon (const std::vector<float>& points);

	bool isInside(glm::vec3) const override;
	// returns whether points A and B are in LOS
	bool intersectSegment(glm::vec2 A, glm::vec2 B, float& t);

	const std::vector<glm::vec2>& getOutline() const;
	const std::vector<glm::vec2>& getHoleOutline(int index) const;
	int getHoleCount() const;

private:
	std::vector<glm::vec2> _points;
	std::vector<std::vector<glm::vec2>> _holes;
};

inline const std::vector<glm::vec2>& Polygon::getOutline() const {
	return _points;
}

inline const std::vector<glm::vec2>& Polygon::getHoleOutline(int index) const {
	return _holes[index];
}

inline int Polygon::getHoleCount() const {
	return _holes.size();
}