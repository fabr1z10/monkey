#include "blink.h"
#include "../node.h"
#include "../components/renderer.h"
#include "../error.h"


Blink::Blink(int id, float totalDuration, float blinkDuration, const pybind11::kwargs& args) : NodeAction(id, args),
	_duration(totalDuration), _blinkDuration(blinkDuration)
{}

void Blink::start() {
	NodeAction::start();
	m_renderer = m_node->getComponent<Renderer>();

	if (m_renderer == nullptr) {
		GLIB_FAIL("Error! No renderer found for " << m_actorId);
	}
	_timer = 0.0f;
	_timer2 = 0.0f;
	m_visible = true;
}

int Blink::process(double dt) {
	_timer += dt;
	_timer2 += dt;
	if (_timer2 >= _blinkDuration) {
		m_visible = !m_visible;
		m_renderer->setState(m_visible ? NodeState::ACTIVE : NodeState::INACTIVE);
		_timer2=0.0f;
	}
	if (_timer >= _duration) {
		m_renderer->setState(NodeState::ACTIVE);
		return 0;
	}
	return 1;




}