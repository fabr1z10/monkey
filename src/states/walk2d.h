#pragma once

#include "../components/statemachine.h"
#include "../components/controller2d.h"
#include "../components/dynamics.h"
#include "../components/sprite_renderer.h"

class Node;

class Walk2D : public State {
public:
	Walk2D() : State() {}
	void start() override;
	void init(const pybind11::kwargs& args) override;
	void setParent(StateMachine*, const pybind11::kwargs&) override;
	void run(double) override;
	//void keyCallback(GLFWwindow*, int key, int scancode, int action, int mods) override;
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
	Controller2D* m_controller;
	Dynamics* m_dynamics;
	Node* m_node;
	Renderer* m_animatedRenderer;
	std::string m_idleAnim;
	std::string m_walkAnim;
	std::string m_jumpAnim;
    std::string m_fallAnim;
	bool m_left;
	bool m_right;
	bool m_up;
};


