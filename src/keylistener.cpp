#include "keylistener.h"
#include "engine.h"

KeyboardListener::KeyboardListener() {
	Engine::instance().registerToKeyboardEvent(this);
}

KeyboardListener::~KeyboardListener() {
	Engine::instance().unregisterToKeyboardEvent(this);
}