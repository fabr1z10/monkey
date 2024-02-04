#pragma once

#include "../runners/scheduler.h"


class Delay : public Action {
public:
	explicit Delay(float t);
	int process(double) override;
	void start() override;
private:
	float m_time;
	float m_timer;
};