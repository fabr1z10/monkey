#include "mariocontroller.h"
#include "../node.h"
#include "../pyhelper.h"
#include <glm/gtx/vector_angle.hpp>
#include <iostream>



MarioController::MarioController(const pybind11::kwargs& args) : Controller(args) {
	m_skinWidth = py_get_dict<float>(args, "skin_width", 0.015f);
	m_maxClimbAngle = glm::radians(py_get_dict<float>(args, "max_climb_angle", 80.0f));

}

std::type_index MarioController::getType() {
	return std::type_index(typeid(Controller));
}


std::shared_ptr<Model> MarioController::getDebugModel() {
	return nullptr;
}


void MarioController::move(glm::vec3& delta, bool forced) {

	_position = m_node->getWorldPosition();
	bool wasGnd = grounded();
	setGrounded(false);

	if (delta.x > 0) {
		float dir_x = m_node->getFlipX() ? -1.f : 1.f;
		glm::vec2 signedDelta = dir_x * delta;

		auto collision = m_collisionEngine->rayCastX(_position, dir_x * delta.x, 0, m_node);
		std::cout << _position.x << ", " << _position.y << ", " << collision.collide << ", " << collision.length << "\n";
		if (collision.collide) {
			auto angle = glm::angle(glm::vec2(collision.normal), glm::vec2(0.f, 1.f));
			if (delta.y < 0 && angle < m_maxClimbAngle) {
				float distance_to_slope_start = collision.length - m_skinWidth / cos(angle);
				float remaining_length = std::max(0.f, delta.x - distance_to_slope_start);
				delta.x = distance_to_slope_start + remaining_length * cos(angle);
				delta.y = remaining_length * sin(angle);
				setGrounded(true);
				_slope = true;
				_slopeNormal = collision.normal;
			} else {
				delta.x = (collision.length - m_skinWidth);

			}
		} else {
			// no collision. check slope down
				if (delta.y < 0 && wasGnd) {
				delta.y = -10.f;
//				auto collide = m_collisionEngine->rayCastY(_position + glm::vec3(signedDelta, 0.f), 10.f, 0, m_node);
//				if (collide.collide) {
//					delta.y = -(collide.length - m_skinWidth);
//					setGrounded(true);
//				}
			}
		}
		_position.x += dir_x * delta.x;
	}


	// first do horizontal
	if (delta.y < 0.f) {
		// going down
		auto collision = m_collisionEngine->rayCastY(_position, delta.y, 0, m_node);
		//std::cout << "y test " << _position.x << ", " << _position.y << ", " << delta.y << ", " << collision.collide << "; " << collision.length << "\n";
		if (collision.collide) {
			delta.y = - (collision.length - m_skinWidth);
			if (!wasGnd) {
				std::cout << "qui\n";
			}
			setGrounded(true);

		}

	}

	m_node->move(delta);


}