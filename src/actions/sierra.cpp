#include "sierra.h"
#include "../node.h"
#include "../components/controllers/sierra2d.h"
#include "../pyhelper.h"

EnableSierraController::EnableSierraController(int id, bool value) : NodeAction(id), _value(value) {

}

void EnableSierraController::start() {
	NodeAction::start();
	m_node->getComponent<PlayerSierra2DController>()->enable(_value);
	m_node->getComponent<Collider>()->setState(_value ? NodeState::ACTIVE : NodeState::PAUSED);
}

ChangeSierraAnim::ChangeSierraAnim(int id, const std::string& idle, const std::string& walk) : NodeAction(id) {
	_idle = idle;
	_walk = walk;

}

void ChangeSierraAnim::start() {
	NodeAction::start();

}