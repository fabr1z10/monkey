#include "foewalk2d.h"
#include "../pyhelper.h"
#include "../node.h"

FoeWalk2D::FoeWalk2D(const pybind11::kwargs &kwargs) : Walk2D(kwargs) {
    m_flipHorizontally = py_get_dict<bool>(kwargs, "flip", false);
    m_flipOnEdge = py_get_dict<bool>(kwargs, "flip_on_edge", false);
}

void FoeWalk2D::control() {

	// hit wall
	if (_keys.x == -1 && m_controller->left()) {
		_keys.x = 1;
	} else if (_keys.x == 1 && m_controller->right()) {
		_keys.x = -1;
	}

	// check if I reached the end of the platform
	if (m_flipOnEdge && m_controller->grounded() && m_controller->isFalling(_keys.x)) {
		// flip x direction
		_keys.x *= -1;
	}

	if (m_flipHorizontally) {
		m_node->setFlipX(_keys.x == -1);
	}



	// update animation, if we have a sprite renderer
	if (m_animatedRenderer) {
		if (m_controller->grounded()) {
			if (fabs(m_dynamics->m_velocity.x) < 0.1f) {
				m_animatedRenderer->setAnimation(m_idleAnim);
			} else {
				m_animatedRenderer->setAnimation(m_walkAnim);
			}
		} else {
			m_animatedRenderer->setAnimation(m_jumpAnim);
		}

	}
}

void FoeWalk2D::setParent(StateMachine* sm, const pybind11::kwargs& kwargs) {
    Walk2D::setParent(sm, kwargs);
	auto left = py_get_dict<bool>(kwargs, "left", true);
	_keys.x = left ? -1 : 1;
	//m_up = py_get_dict<bool>(kwargs, "up", false);
}


