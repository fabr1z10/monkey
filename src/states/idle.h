#pragma once

#include "../components/statemachine.h"
#include "../components/sprite_renderer.h"

class Idle : public State {
public:
    Idle() : State(){}


	void init(const pybind11::kwargs& args) override;
	void setParent(StateMachine*, const pybind11::kwargs&) override;
	void run(double) override;

private:
	std::string m_animation;
	SpriteRenderer* m_spriteRenderer;
	std::string m_exitState;
	float m_timeOut;
	float m_time;
	bool m_exitOnComplete;

};


