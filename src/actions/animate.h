#pragma once

#include "nodeaction.h"
#include "../components/sprite_renderer.h"

class Animate : public NodeAction {
public:
	Animate(int id, const std::string& animation, bool sync, bool backwards);
	void start() override;
	int process(double) override;
private:
	SpriteRenderer* m_renderer;
	std::string m_animation;
	bool m_sync;
	bool _back;
};
