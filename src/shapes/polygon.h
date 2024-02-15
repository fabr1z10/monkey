#pragma once

#include "compound.h"
#include <vector>


// generic polygon, can be concave
// can have holes
class GenericPolygon : public CompoundShape {
public:
	GenericPolygon (const std::vector<float>& points);

	bool isInside(glm::vec3) const override;
	// returns whether points A and B are in LOS
	bool intersectSegment(glm::vec2 A, glm::vec2 B, float& t);

	const std::vector<glm::vec2>& getOutline() const;
	const std::vector<glm::vec2>& getHoleOutline(int index) const;
	int getHoleCount() const;
	void addHole(const std::vector<float>& points);
	void
private:
	std::vector<glm::vec2> _points;
	std::vector<std::vector<glm::vec2>> _holes;
};

inline const std::vector<glm::vec2>& GenericPolygon::getOutline() const {
	return _points;
}

inline const std::vector<glm::vec2>& GenericPolygon::getHoleOutline(int index) const {
	return _holes[index];
}

inline int GenericPolygon::getHoleCount() const {
	return _holes.size();
}