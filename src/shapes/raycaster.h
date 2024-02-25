#pragma once

#include <unordered_map>
#include <functional>
#include "../shape.h"
#include "glm/glm.hpp"
#include "../hashpair.h"
#include "../runners/collision.h"


enum Direction {
	X, Y, Z
};


class RayCaster {
public:
    RayCaster();
    virtual ~RayCaster() = default;
    RayCastHit raycast(glm::vec3 A, glm::vec3 B, const Shape*, const glm::mat4&);

    /// cast a ray along an axis
    RayCastHit raycastX(glm::vec3 P, float length, const Shape*, const glm::mat4&);
	RayCastHit raycastY(glm::vec3 P, float length, const Shape*, const glm::mat4&);
    RayCastHit raycastZ(glm::vec3 P, float length, const Shape*, const glm::mat4&);
	//RayCastHit raycastX(glm::vec3 P, float length, const Shape*, const glm::mat4&);


protected:
	static std::vector<glm::ivec3> _axes;
    std::unordered_map<std::type_index, std::function<RayCastHit(const glm::vec3& A, const glm::vec3& B, const Shape*, const glm::mat4&)>> m_functionMap;
	std::unordered_map<std::type_index, std::function<RayCastHit(const glm::vec3& P, float, const Shape*, const glm::mat4&)>>
		m_functionsX;
	std::unordered_map<std::type_index, std::function<RayCastHit(const glm::vec3& P, float, const Shape*, const glm::mat4&)>>
		m_functionsY;
    std::unordered_map<std::type_index, std::function<RayCastHit(const glm::vec3& P, float, const Shape*, const glm::mat4&)>>
            m_functionsZ;
};

class RayCaster2D : public RayCaster {
public:
	RayCaster2D();
private:
    RayCastHit rayCastAABB(const glm::vec3& A, const glm::vec3& B, const Shape *s, const glm::mat4 &t);
	RayCastHit rayCastPoly(const glm::vec3& A, const glm::vec3& B, const Shape *s, const glm::mat4 &t);
    RayCastHit rayCastPolyline(const glm::vec3& A, const glm::vec3& B, const Shape *s, const glm::mat4 &t);
	// all shapes default to this, except circle and aabb that have special handlers
	RayCastHit rayCastXGeneric(const glm::vec3& P, float, const Shape*, const glm::mat4& t);
	RayCastHit rayCastYGeneric(const glm::vec3& P, float, const Shape*, const glm::mat4& t);

	RayCastHit rayCastAxid2DAABB(const glm::vec3& P, Direction, float, const Shape*, const glm::mat4& t);
	void updateRaycastHit(RayCastHit& r, glm::vec2 ray, glm::vec2 line, float u, int si);
};