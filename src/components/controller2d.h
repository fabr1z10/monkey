#pragma once

#include "controller.h"
#include "platform.h"

struct RaycastOrigins {
	glm::vec3 topFwd, topBack;
	glm::vec3 bottomFwd, bottomBack;
};


// this is for 2D platformers
class Controller2D : public Controller {
public:
	Controller2D(const pybind11::kwargs&);
	virtual ~Controller2D();
	void move(glm::vec3&, bool forced) override;
	std::type_index getType() override;
	void updateRaycastOrigins() override;
	bool isFalling(float) override;

	//	bool grounded() const override;
//	bool ceiling() const;
//	bool left() const;
//	bool right() const;
	void setPlatform(Platform*);
	void resetPlatform();
	void resetDetails();
	void resetCollisions() override;
private:
	std::shared_ptr<Model> getDebugModel() override;

	struct CollisionDetails {
		bool above, below;
		bool left, right;
		bool climbingSlope;
		bool descendingSlope;
		glm::vec3 velocityOld;
		float slopeAngle, slopeAngleOld;
		void reset();
	};

	void climbSlope(glm::vec3& velocity, float slopeAngle);
	void descendSlope(glm::vec3&);
	void horizontalCollisions(glm::vec3& velocity);
	void verticalCollisions(glm::vec3& velocity, bool);

	RaycastOrigins m_raycastOrigins;

	bool m_wasGnd;
	//CollisionDetails m_details;
	float m_maxClimbAngle;
	float m_maxDescendAngle;
	float m_skinWidth;
	bool m_faceRight;
	int m_horizontalRayCount;
	int m_verticalRayCount;
	float m_horizontalRaySpacing;
	float m_verticalRaySpacing;

	// platforms on which I registered
	Platform* m_platforms;

	bool m_climbingSlope;
	bool m_descendingSlope;
	glm::vec3 m_velocityOld;
	float m_slopeAngle, m_slopeAngleOld;
	int _foeFlag;

	int _platformFlag;
};

//inline bool Controller2D::grounded() const {
//	return m_details.below;
//}
//
//inline bool Controller2D::ceiling () const {
//	return m_details.above;
//}
//
//inline bool Controller2D::left () const {
//	return m_details.left;
//}
//inline bool Controller2D::right () const {
//	return m_details.right;
//}

