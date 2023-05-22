#pragma once

#include "walk2d.h"
#include "../components/controller.h"
#include "../components/dynamics.h"
#include "../components/sprite_renderer.h"

class Node;

class Climb : public State {
public:
	Climb() : State() {}
    void setParent(StateMachine*, const pybind11::kwargs& kwargs) override;
	void start() override;
	void run(double) override;
	void init(const pybind11::kwargs& args) override;
	void end() override;

private:
	Controller2D* m_controller;
	Dynamics* m_dynamics;
	Node* m_node;
	SpriteRenderer* m_animatedRenderer;
	std::string m_animation;
	std::string m_animIdle;
	float m_speed;
	std::string m_walkState;
	glm::ivec2 m_mask;

	int m_maskUp;
	int m_maskDown;

};


