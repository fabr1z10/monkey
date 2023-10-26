#include "controller2d.h"
#include "../shapes/aabb.h"
#include "../models/modelmake.h"
#include "../pyhelper.h"
#include "../node.h"
#include "../util.h"


Controller2D::Controller2D(const pybind11::kwargs& kwargs) : Controller(kwargs) {
	m_maxClimbAngle = glm::radians(py_get_dict<float>(kwargs, "max_climb_angle", 80.0f));
	m_maxDescendAngle = glm::radians(py_get_dict<float>(kwargs, "max_descend_angle", 80.0f));
	m_skinWidth = py_get_dict<float>(kwargs, "skinWidth", .015f);
	m_horizontalRayCount = py_get_dict<int>(kwargs, "horizontal_ray_count", 4);
	m_verticalRayCount = py_get_dict<int>(kwargs, "vertical_ray_count", 4);
	m_maskDown = py_get_dict<int>(kwargs, "mask_down", 2 | 32);
	m_maskUp = py_get_dict<int>(kwargs, "mask_up", 2);
	m_platforms = nullptr;
}

Controller2D::~Controller2D() {
	if (m_platforms != nullptr) {
		m_platforms->unregisterComponent(this);
		m_platforms->forceRemove(this);
	}
}

std::shared_ptr<Model> Controller2D::getDebugModel() {
	auto shape = std::make_shared<AABB>(-m_center.x, -m_center.x + m_size.x, -m_center.y, -m_center.y + m_size.y);
	auto& m = ModelMaker::instance();
	auto model = m.make(shape, glm::vec4(1.f, 0.f, 0.f, 1.f), FillType::OUTLINE);
	return model;
}

void Controller2D::updateRaycastOrigins() {
	auto worldMatrix = m_node->getWorldMatrix();
	m_raycastOrigins.topFwd = worldMatrix * glm::vec4(m_localTopFwd, 1.0f);
	m_raycastOrigins.topBack = worldMatrix * glm::vec4(m_localTopBack, 1.0f);
	m_raycastOrigins.bottomFwd = worldMatrix * glm::vec4(m_localBottomFwd, 1.0f);
	m_raycastOrigins.bottomBack = worldMatrix * glm::vec4(m_localBottomBack, 1.0f);
	float width = fabs(m_raycastOrigins.topFwd.x - m_raycastOrigins.topBack.x);
	float height = m_raycastOrigins.topFwd.y - m_raycastOrigins.bottomFwd.y;
	m_horizontalRaySpacing = height / (m_horizontalRayCount - 1.f);
	m_verticalRaySpacing = width / (m_verticalRayCount - 1.f);
	//std::cout << "bottom fwd/back " << m_raycastOrigins.bottomFwd.x << "; " << m_raycastOrigins.bottomBack.x << "\n";
}

void Controller2D::move(glm::vec3& delta, bool forced) {

	if (delta == glm::vec3(0.0f)) return;

	updateRaycastOrigins();

	// check if character was grounded at last iteration
	m_wasGnd = m_details.below;
	m_details.reset();
	if (forced) {
		m_details.below = true;
	}

	if (delta.y < 0 && m_wasGnd) {
		descendSlope(delta);
	}
	if (!isEqual(delta.x, 0.0f))
		horizontalCollisions(delta);
	if (!isEqual(delta.y, 0.0f))
		verticalCollisions(delta, forced);

	//m_node->move(glm::translate(delta));
	m_node->move(delta);

}


void Controller2D::CollisionDetails::reset() {
	above = below = false;
	left = right = false;
	climbingSlope = false;
	descendingSlope = false;
	slopeAngleOld = slopeAngle;
	slopeAngle = 0.0f;
}

void Controller2D::descendSlope(glm::vec3& velocity) {
	if (velocity.x == 0.0f) return;
	bool goingForward = velocity.x > 0.0f;
	auto directionX = signf(velocity.x);
	auto r0 = directionX > 0.f ? m_raycastOrigins.bottomFwd : m_raycastOrigins.bottomBack;
	RayCastHit hit = m_collisionEngine->rayCast(r0, glm::vec3(0.f, -1.0f, 0.f), 100.0f, m_maskDown);
	if (hit.collide) {
		float slopeAngle = angle(hit.normal, glm::vec3(0.f, 1.f, 0.f));
		if (slopeAngle != 0 && slopeAngle <= m_maxDescendAngle) {
			if (signf(hit.normal.x) == directionX) {
				if (hit.length - m_skinWidth <= tan(slopeAngle) * fabs(velocity.x)) {
					float moveDistance = fabs(velocity.x);
					float descendVelocityY = sin(slopeAngle) * moveDistance;
					velocity.x = directionX *cos(slopeAngle) * moveDistance;// * sign(velocity.x);
					velocity.y -= descendVelocityY;
					m_details.slopeAngle = slopeAngle;
					m_details.descendingSlope = true;
					m_details.below = true;
				}
			}
		}
	}
}

void Controller2D::horizontalCollisions(glm::vec3& velocity) {
	float directionX = signf(velocity.x);
	bool goingForward = directionX > 0.f;
	float rayLength = fabs(velocity.x) + m_skinWidth;
	auto r0 = directionX > 0.f ? m_raycastOrigins.bottomFwd : m_raycastOrigins.bottomBack;
	float dir_x = (goingForward == m_node->getFlipX()) ? -1.f : 1.f;
	for (int i = 0; i < m_horizontalRayCount; i++) {
		auto rayOrigin = r0 + glm::vec3(0.0f, i * m_horizontalRaySpacing, 0.0f);
		RayCastHit hit = m_collisionEngine->rayCast(rayOrigin, glm::vec3(dir_x, 0.f, 0.f), rayLength, 2 | 32);
		if (hit.collide) {
			float slopeAngle = angle(hit.normal, glm::vec3(0.f, 1.f, 0.f));
			if (i == 0 && slopeAngle <= m_maxClimbAngle) {
				if (m_details.descendingSlope) {
					m_details.descendingSlope = false;
					velocity = m_details.velocityOld;
				}
				float distanceToSlopeStart = 0;
				if (slopeAngle != m_details.slopeAngleOld) {
					distanceToSlopeStart = hit.length - m_skinWidth;
					velocity.x -= distanceToSlopeStart * directionX;
				}
				climbSlope(velocity, slopeAngle);
				velocity.x += distanceToSlopeStart * directionX;
			}
			if (!m_details.climbingSlope || slopeAngle > m_maxClimbAngle) {
				velocity.x = std::max(hit.length - m_skinWidth, 0.0f) * directionX;
				rayLength = hit.length;
				if (m_details.climbingSlope) {
					velocity.y = tan(m_details.slopeAngle)* fabs(velocity.x);
				}
				bool faceRight = !m_node->getFlipX();
				auto dir_x = (goingForward == m_node->getFlipX()) ? -1 : 1;
				m_details.left = dir_x == -1;
				m_details.right = dir_x == 1;
			}
		}
	}
}

void Controller2D::setMask(int up, int down) {
	m_maskUp = up;
	m_maskDown = down;
}

glm::ivec2 Controller2D::getMask() const {
	return glm::ivec2(m_maskUp, m_maskDown);
}

void Controller2D::verticalCollisions(glm::vec3& velocity, bool forced) {
	auto directionY = signf(velocity.y);
	bool goingForward = velocity.x > 0.0f;
	bool faceRight = !m_node->getFlipX();
	float dir_x = (goingForward == m_node->getFlipX()) ? -1.f : 1.f;
	float directionX = (goingForward == m_faceRight) ? 1.f : -1.f;
	float rayLength = std::abs(velocity.y) + m_skinWidth;
	float obstacleDistance = std::numeric_limits<float>::max();
	//std::unordered_set<Node*> obstacles;
	Node* obstacle = nullptr;
	float speedX = fabs(velocity.x);
	bool atleast = false;
	//glm::vec3 r0(m_raycastOrigins.xMin, 0.f, 0.f);
	glm::vec3 r0 = directionY > 0.f ? (faceRight ? m_raycastOrigins.topBack : m_raycastOrigins.topFwd) :
				   (faceRight ? m_raycastOrigins.bottomBack : m_raycastOrigins.bottomFwd);
	for (int i = 0; i < m_verticalRayCount; i++) {
		auto rayOrigin = r0 + glm::vec3(i * m_verticalRaySpacing + dir_x * velocity.x, 0.f, 0.f) ;
		int collMask = (directionY == -1 ? (m_maskDown) : m_maskUp);
		RayCastHit hit = m_collisionEngine->rayCast(rayOrigin, glm::vec3(0.f, directionY, 0.f), rayLength, collMask);
		bool ciao = m_details.below;
		if (hit.collide) {
			if (!m_wasGnd) {
			}
			atleast = true;
			velocity.y = (hit.length - m_skinWidth) * directionY;
			rayLength = hit.length;
			if (m_details.climbingSlope) {
				velocity.x = (velocity.y / tan(m_details.slopeAngle)) * signf(velocity.x);
			}
			m_details.below = directionY == -1;
			m_details.above = directionY == 1;
			if (directionY < 0.0f) {
				if (hit.length < obstacleDistance) {
					obstacle = hit.entity->getNode();
					obstacleDistance = hit.length;
				}
				//obstacles.insert(hit.entity->getNode());
			}
		}
	}


	// register to new platforms -- needed for moving platforms+
	if (obstacle != nullptr) {
		auto* platformController = obstacle->getComponent<Platform>();

		if (platformController != m_platforms) {
			if (platformController != nullptr) {
				platformController->registerComponent(this);
			}
			if (m_platforms != nullptr) {
				m_platforms->unregisterComponent(this);
			}
			m_platforms = platformController;
		}
	} else {
		if (m_platforms != nullptr && !forced) {
			m_platforms->unregisterComponent(this);
			m_platforms = nullptr;
		}
	}

}

void Controller2D::setPlatform(Platform * p) {
	m_details.below = true;
	if (m_platforms != nullptr && m_platforms != p) {
		m_platforms->unregisterComponent(this);
	}
	m_platforms = p;
}

void Controller2D::resetPlatform() {
	m_platforms = nullptr;
}

std::type_index Controller2D::getType() {
	return std::type_index(typeid(Controller));
}


void Controller2D::climbSlope(glm::vec3& velocity, float slopeAngle) {
	float moveDistance = fabs(velocity.x);
	float climbVelocityY = sin(slopeAngle) * moveDistance;
	if (velocity.y <= climbVelocityY) {
		velocity.y = climbVelocityY;
		velocity.x = cos(slopeAngle) * moveDistance * signf(velocity.x);
		m_details.below = true;
		m_details.climbingSlope = true;
		m_details.slopeAngle = slopeAngle;
	}
}

bool Controller2D::isFalling(float dir) {
	updateRaycastOrigins();
	auto rayOrigin = m_raycastOrigins.bottomFwd; //(dir < 0.f ? m_raycastOrigins.bottomLeft : m_raycastOrigins.bottomRight);
	rayOrigin.x += (m_node->getFlipX() ? -1.f : 1.f) * 8.f;
	//glm::vec2 rayOrigin = (dir == -1) ? m_raycastOrigins.bottomLeft : m_raycastOrigins.bottomRight;
	RayCastHit hit = m_collisionEngine->rayCast(rayOrigin, glm::vec3(0.f, -1.f, 0.f), 0.5f, 2|32);
	if (!hit.collide) {
		return true;
	}
	return false;
}

void Controller2D::resetDetails() {
	m_details.reset();
}