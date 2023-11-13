#pragma once

#include "../components/statemachine.h"
#include "../components/controller.h"
#include "../components/dynamics.h"
#include "../components/sprite_renderer.h"



class Top2D : public State {
public:
	Top2D(const pybind11::kwargs&);
	//void setParent(StateMachine*, const pybind11::kwargs&) override;
	void start() override;
	void run(double) override;
	struct Keys {
		Keys() {x=0; y=0;}
		int x;
		int y;
	};
protected:
	Keys _keys;
	float _maxSpeed;
	float _accelerationTime;
	float _acceleration;
	Controller* m_controller;
	Dynamics* m_dynamics;
	float _rotationSpeed;
	Node* m_node;
	Renderer* m_animatedRenderer;
};