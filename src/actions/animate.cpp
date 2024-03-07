#include "animate.h"
#include "../pyhelper.h"
#include "../node.h"

Animate::Animate(int id, const std::string& animation, bool sync, bool backwards) : NodeAction(id),
	m_animation(animation), m_sync(sync), _back(backwards){



}

void Animate::start() {
	NodeAction::start();
	m_renderer = dynamic_cast<SpriteRenderer*>(m_node->getComponent<Renderer>());
	assert(m_renderer != nullptr);
	m_renderer->setAnimationForce(m_animation);
	m_renderer->setDirection(_back ? -1 : 1);

}

int Animate::process(double) {
	if (!m_sync) {
		return 0;
	}
	return (m_renderer->isComplete() ? 0 : 1);
}