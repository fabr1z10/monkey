#pragma once

#include "../components/statemachine.h"
#include "../components/controller2d.h"
#include "../components/dynamics.h"
#include "../components/sprite_renderer.h"

class Node;

class Bounce : public State {
public:
	Bounce() : State() {}
	void run(double) override;
	void init(const pybind11::kwargs& args) override;
	void setParent(StateMachine*, const pybind11::kwargs&) override;


private:
	Controller2D* m_controller;
	Dynamics* m_dynamics;
	float m_gravity;
	Node* m_node;
	pybind11::function m_onBounceY;
	int m_bounceCountY;
	int m_counter;
	std::vector<float> m_fixedVelocityBounce;
	bool m_isFixedVel;
	bool m_checkWalls;
	SpriteRenderer* m_animatedRenderer;
	std::string m_animation;
	bool m_flipHorizontally;
	bool m_left;
	float m_horizontalSpeed;
	bool m_flipOnEdge;

};


