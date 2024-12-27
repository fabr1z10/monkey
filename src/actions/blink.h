#pragma once

#include "nodeaction.h"

class Renderer;

class Blink : public NodeAction {
public:
	//! \name Constructors
	//@{
	/*! Blink the given actor for a certain time, each blink lasting t.
	 */
	Blink(int id, float totalDuration, float blinkDuration, const pybind11::kwargs& args);
	//@}
	void start() override;
	int process (double dt) override ;

private:
	float _timer;
	float _timer2;
	bool m_visible;
	float _duration;
	float _blinkDuration;
	Renderer* m_renderer;
	std::string m_actorId;
};