#include "mouselistener.h"
#include "engine.h"

MouseListener::MouseListener() {
	Engine::instance().registerToMouseEvent(this);

}

MouseListener::~MouseListener() {
	Engine::instance().unregisterToMouseEvent(this);

}