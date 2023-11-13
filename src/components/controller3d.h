#pragma once

#include "controller.h"

#include <glm/glm.hpp>

class ICollisionEngine;
class ICollider;
class PlatformComponent;

struct CollisionDetails3D {
	CollisionDetails3D();

	bool above, below;
	bool left, right;
	bool front, back;

	//bool climbingSlope;
	//bool descendingSlope;

	//float slopeAngle, slopeAngleOld;
	void Reset();
};

inline void CollisionDetails3D::Reset() {
	above = below = false;
	left = right = false;
	front = back  = false;

//	climbingSlope = false;
//	descendingSlope = false;
//	slopeAngleOld = slopeAngle;
//	slopeAngle = 0.0f;
}

struct RaycastOrigins3D {
	RaycastOrigins3D();

	float fwd, rear;
	float bottom, top;
	float front, back;
};


class Controller3D : public Controller {
public:
	explicit Controller3D(const pybind11::kwargs& args);
	virtual ~Controller3D() {}
	std::shared_ptr<Model> getDebugModel() override;
	void move(glm::vec3&, bool forced) override;
	void updateRaycastOrigins();
	bool isFalling(float) override;

	bool IsFalling(int x, int z);
	//bool grounded() const override;
	//bool ceiling() const override;
	//bool ceiling () const override;
	//bool side () const override {return false;}


	void CalculateRaySpacing();
	void update(double) override {}
	CollisionDetails3D m_details;
	RaycastOrigins3D m_raycastOrigins;
	std::type_index getType() override;
	//void ClimbSlope(glm::vec3&, float);
    //void ClimbSlopeZ(glm::vec3&, float);

	//RayCastHit2D Raycast(glm::vec2 origin, glm::vec2 direction, float length, int mask);
private:
	//void drawShape() override;

	//std::vector<Collider*> m_ppp;
	void horizontalCollisions(glm::vec3& velocity);
	void verticalCollisions(glm::vec3& velocity);
	void UpdateRaycastOrigins();
	//ICollisionEngine * m_collision;
	int _maskPlatform;


	//ICollider* m_cc;
	float m_skinWidth;
	//float m_maxClimbAngle;
	//float m_maxDescendAngle;

};

inline std::type_index Controller3D::getType() {
	return std::type_index(typeid(Controller));
}

//inline bool Controller3D::grounded() const {
//	return m_details.below;
//}
//
//inline bool Controller3D::ceiling() const {
//	return m_details.above;
//}