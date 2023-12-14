#pragma once

#include "nodeaction.h"

class Renderer;

class Blink : public NodeAction {
public:
	explicit Blink(int id, float duration, float period);
	int process(double) override;
	void start() override;
	void onEnd() override;
private:
	float m_duration;
	float m_period;
	float m_time;
	Renderer* m_renderer;

};