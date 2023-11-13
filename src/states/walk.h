#pragma once

#include "../components/statemachine.h"
#include "../components/controller.h"
#include "../components/dynamics.h"
#include "../components/sprite_renderer.h"

struct Keys {
	Keys();
	int x;
	int z;
	bool jump;
	bool crouch;
};

class Walk : public State {
public:
	Walk(const pybind11::kwargs&);
	void setParent(StateMachine*, const pybind11::kwargs&) override;
	void start() override;
	virtual void control() = 0;
protected:
	float m_gravity;
	float m_jumpHeight;
	float m_timeToJumpApex;
	float m_jumpVelocity;           // calculated
	float m_acceleration;
	float m_maxSpeedGround;
	float m_maxSpeedAir;
	float m_accelerationTime;
	Controller* m_controller;
	Dynamics* m_dynamics;
	Node* m_node;
	Renderer* m_animatedRenderer;
	std::string m_idleAnim;
	std::string m_walkAnim;
	std::string m_jumpAnim;
	std::string m_fallAnim;
	Keys _keys;
	// arrow keys
	//bool m_left;
	//bool m_right;
	//bool m_up;
	//bool m_down;
	bool m_isJumping;
};