#pragma once

#include <unordered_map>
#include <functional>
#include "../shape.h"
#include "glm/glm.hpp"
#include "../hashpair.h"
#include "../runners/collision.h"


class RayCaster {
public:
    RayCaster() = default;
    virtual ~RayCaster() = default;
    RayCastHit raycast(glm::vec3 A, glm::vec3 B, const Shape*, const glm::mat4&);
protected:
    std::unordered_map<std::type_index, std::function<RayCastHit(const glm::vec3& A, const glm::vec3& B, const Shape*, const glm::mat4&)>> m_functionMap;


};

class RayCaster2D : public RayCaster {
public:
	RayCaster2D();
private:
	RayCastHit rayCastPoly(const glm::vec3& A, const glm::vec3& B, const Shape *s, const glm::mat4 &t);
	void updateRaycastHit(RayCastHit& r, glm::vec2 ray, glm::vec2 line, float u, int si);
};