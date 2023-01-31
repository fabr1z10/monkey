#pragma once

#include "walk2d.h"
#include "../components/controller.h"
#include "../components/dynamics.h"
#include "../components/sprite_renderer.h"

class Node;

class Climb : public State {
public:
	Climb(const std::string& id, const pybind11::kwargs&);
	void run(double) override;
	void init(const pybind11::kwargs& args) override;

	void setParent(StateMachine*) override;
private:
	Controller2D* m_controller;
	Dynamics* m_dynamics;
	Node* m_node;
	SpriteRenderer* m_animatedRenderer;
	std::string m_animation;
	std::string m_animIdle;
	float m_speed;
};


