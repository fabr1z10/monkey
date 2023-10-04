#include "animate.h"
#include "../pyhelper.h"
#include "../node.h"

Animate::Animate(const pybind11::kwargs& args) : NodeAction(args) {
	m_animation = args["anim"].cast<std::string>();
	m_sync = py_get_dict<bool>(args, "sync", false);

}

void Animate::start() {
	NodeAction::start();
	m_renderer = dynamic_cast<SpriteRenderer*>(m_node->getComponent<Renderer>());
	assert(m_renderer != nullptr);
	m_renderer->setAnimationForce(m_animation);

}

int Animate::process(double) {
	if (!m_sync) {
		return 0;
	}
	return (m_renderer->isComplete() ? 0 : 1);
}