#pragma once

#include "controller.h"
#include "platform.h"
#include "mover.h"

struct RaycastOrigins {
	glm::vec2 topFwd, topBack;
	glm::vec2 bottomFwd, bottomBack;
};


// this is for 2D platformers
class Controller2D : public Controller {
public:
	Controller2D(const pybind11::kwargs&);
	virtual ~Controller2D();


    void move(glm::vec2&, bool forced) override;
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
protected:
	//std::shared_ptr<Model> getDebugModel() override;

	struct CollisionDetails {
		bool above, below;
		bool left, right;
		bool climbingSlope;
		bool descendingSlope;
		glm::vec3 velocityOld;
		float slopeAngle, slopeAngleOld;
		void reset();
	};

	void climbSlope(glm::vec2& velocity, float slopeAngle);
	void descendSlope(glm::vec2&);
	void horizontalCollisions(glm::vec2& velocity);
	void verticalCollisions(glm::vec2& velocity, bool);

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
	Mover* _mover;

	bool m_climbingSlope;
	bool m_descendingSlope;
	glm::vec3 m_velocityOld;
	float m_slopeAngle, m_slopeAngleOld;
	int _foeFlag;

	int _platformFlag;
	int _direction;

    float _acc;
    float _maxSpeed;
	float _jumpHeight;
	float _timeToJumpApex;
	float _gravity;
	float _jumpVelocity;
    glm::vec2 _velocity;
    glm::vec2 _acceleration;
};


class PlayerController2D : public Controller2D {
public:
	PlayerController2D(const pybind11::kwargs& args) : Controller2D(args) {}

	void update(double) override;

};


class CustomController2D : public Controller2D {
public:
	CustomController2D(const pybind11::function callback, const pybind11::kwargs& args) : Controller2D(args), _callback(callback) {}

	void update(double) override;
private:
	pybind11::function _callback;
};
