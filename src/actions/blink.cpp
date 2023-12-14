#include "blink.h"
#include "../node.h"
#include "../components/renderer.h"

Blink::Blink(int id, float duration, float period) :
	NodeAction(id), m_duration(duration), m_period(period) {

}

void Blink::start() {
	NodeAction::start();
	m_renderer = m_node->getComponent<Renderer>();
	m_time = 0.f;
}

void Blink::onEnd() {
	//m_renderer->setMultColor(glm::vec4(1.f));

}

int Blink::process(double dt) {
	m_time += static_cast<float>(dt);
	if (m_time > m_duration) {
		return 0.f;
	}
	int a = static_cast<int>(m_time / m_period) % 2;
	//m_renderer->setMultColor(a == 0 ? glm::vec4(1.f) : glm::vec4(0.f));

	return 1.f;
}
