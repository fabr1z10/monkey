#include "delay.h"

Delay::Delay(float t) : Action(), m_time(t) {}

void Delay::start() {
	m_timer = 0.f;
}

int Delay::run(double dt) {
	m_timer += static_cast<float>(dt);
	if (m_timer > m_time) {
		return 0;
	}
	return 1;
}