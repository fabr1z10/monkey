#include "scriptplayer.h"

ScriptPlayer::ScriptPlayer() : Component() {}

void ScriptPlayer::update(double dt) {
	m_script->update(dt);
}

void ScriptPlayer::play(std::shared_ptr<Script> script) {

	m_script = script;
}