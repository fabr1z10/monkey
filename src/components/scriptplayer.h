#pragma once

#include "../component.h"
#include "../runners/scheduler.h"

class ScriptPlayer : public Component {
public:
	ScriptPlayer();
	void play(std::shared_ptr<Script> script);
	void update(double) override;
private:
	std::shared_ptr<Script> m_script;
};