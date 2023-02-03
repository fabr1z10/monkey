#pragma once

#include "../runners/scheduler.h"


class Delay : public Action {
public:
	Delay(float t);
	int run(double) override;
	void start() override;
private:
	float m_time;
	float m_timer;
};