#pragma once

#include "controller.h"
#include "platform.h"
#include "mover.h"
#include "../keylistener.h"
#include "controller_state.h"


struct RaycastOrigins {
	glm::vec2 topFwd, topBack;
	glm::vec2 bottomFwd, bottomBack;
};


// this is for 2D platformers
class Controller2D : public Controller {
public:
	Controller2D(const pybind11::kwargs&);
	virtual ~Controller2D();
	void start() override;
	void update(double) override;
	void shutdown() override;

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
	void setState(int);
	int getState() const;
	float getGravity() const;

	int addState(pybind11::object obj);
	float getAcceleration() const;
	float getMaxSpeed() const;
	//glm::vec2 getVelocity() const;
	//void setVelocity(glm::vec2);
	float getJumpVelocity() const;
protected:
	//std::shared_ptr<Model> getDebugModel() override;
	std::vector<pybind11::object> _pythonObj;
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

    float _acc;
    float _maxSpeed;
	float _jumpHeight;
	float _timeToJumpApex;
	float _gravity;
	float _jumpVelocity;
    //glm::vec2 _velocity;
    //glm::vec2 _acceleration;

    int _state;
	//std::vector<std::function<void(double)>> _controllers;
    std::vector<std::shared_ptr<ControllerState>> _controllers;
};

inline float Controller2D::getMaxSpeed() const {
	return _maxSpeed;
}

inline float Controller2D::getAcceleration() const {
	return _acc;
}

inline  float Controller2D::getGravity() const {
	return _gravity;
}

inline void Controller2D::setState(int state) {
	if (_state != state) {
		_state = state;
		_controllers[_state]->start();
	}
}

inline int Controller2D::getState() const {
	return _state;
}

//inline glm::vec2 Controller2D::getVelocity() const {
//	return _velocity;
//}
//
//inline void Controller2D::setVelocity(glm::vec2 velocity) {
//	_velocity = velocity;
//}

inline float Controller2D::getJumpVelocity() const {
	return _jumpVelocity;
}



class PlayerController2D : public Controller2D, public KeyboardListener {
public:
	PlayerController2D(const std::string& batch, const pybind11::kwargs& args);

	void start() override;

	void setNode(Node*) override;
	void defaultController(double);
	void setModel(int index);
	void addModel(std::shared_ptr<Model>, const std::string& idleAnimation, const std::string& walkAnimation,
		const std::string& slideAnimation, const std::string& jumpUpAnimation, const std::string& jumpDownAnimation);
	int keyCallback(GLFWwindow*, int key, int scancode, int action, int mods) override;
	void addKeyCallback(int, pybind11::function);

private:


	int _currentModel;
	std::string _batch;
	struct ModelInfo {
		std::shared_ptr<Model> model;
		std::string walk;
		std::string idle;
		std::string jumpUp;
		std::string jumpDown;
		std::string slide;
	};
	std::unordered_map<int, pybind11::function> _callbacks;
	std::vector<ModelInfo> _models;

public:
	const ModelInfo& getModelInfo() const;

};

inline const PlayerController2D::ModelInfo & PlayerController2D::getModelInfo() const {
	return _models[_currentModel];
}

class PlayerControllerState : public ControllerState {
public:
	void init(Node*) override;

	void update(double) override;
private:
	PlayerController2D* _controller;
	glm::vec2 _velocity;
	int _direction;

};
