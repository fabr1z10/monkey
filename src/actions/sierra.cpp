#include "sierra.h"
#include "../node.h"
#include "../components/controllers/sierra2d.h"
#include "../pyhelper.h"

EnableSierraController::EnableSierraController(const pybind11::kwargs &args) : NodeAction(args) {
	_value = py_get_dict<bool>(args, "value");
}

void EnableSierraController::start() {
	NodeAction::start();
	m_node->getComponent<Sierra2DController>()->enable(_value);
}

ChangeSierraAnim::ChangeSierraAnim(const pybind11::kwargs &args) : NodeAction(args) {
	_idle = py_get_dict<std::string>(args, "idle");
	_walk = py_get_dict<std::string>(args, "walk");

}

void ChangeSierraAnim::start() {
	NodeAction::start();

}