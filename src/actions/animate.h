#pragma once

#include "nodeaction.h"
#include "../components/sprite_renderer.h"

class Animate : public NodeAction {
public:
	Animate(const pybind11::kwargs&);
	void start() override;
	int run(double) override;
private:
	SpriteRenderer* m_renderer;
	std::string m_animation;
	bool m_sync;
};
