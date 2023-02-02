#pragma once

#include "../components/statemachine.h"
#include "../components/sprite_renderer.h"

class Idle : public State {
public:
	Idle(const std::string& id, const std::string& anim, const pybind11::kwargs&);
	void init(const pybind11::kwargs& args) override;
	void setParent(StateMachine*) override;
	void run(double) override;

private:
	std::string m_animation;
	SpriteRenderer* m_spriteRenderer;
	std::string m_exitState;
	float m_timeOut;
	float m_time;
	bool m_exitOnComplete;

};


