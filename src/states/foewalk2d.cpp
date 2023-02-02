#include "foewalk2d.h"
#include "../pyhelper.h"
#include "../node.h"

void FoeWalk2D::control() {

	// hit wall
	if ( (m_left && m_controller->left()) || (m_right && m_controller->right())) {
		m_left = !m_left;
		m_right = !m_left;
	}

	// check if I reached the end of the platform
	if (m_flipOnEdge && m_controller->grounded() && m_controller->isFalling(m_left ? -1.f : 1.f)) {
		m_left = !m_left;
		m_right = !m_left;

	}

	if (m_flipHorizontally) {
		m_node->setFlipX(m_left);
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

FoeWalk2D::FoeWalk2D(const std::string& id, const pybind11::kwargs& kwargs) : Walk2D(id, kwargs) {
	m_left = py_get_dict<bool>(kwargs, "left", true);
	m_flipHorizontally = py_get_dict<bool>(kwargs, "flip", false);
	m_flipOnEdge = py_get_dict<bool>(kwargs, "flip_on_edge", false);
	m_right = !m_left;
	m_up = py_get_dict<bool>(kwargs, "up", false);
}


