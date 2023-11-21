//#include "controller3d.h"
//#include "../pyhelper.h"
//#include "../shapes3d/aabb3d.h"
//#include "../models/modelmake.h"
//#include "../node.h"
//#include "../util.h"
//
//
//CollisionDetails3D::CollisionDetails3D() : above(false), below(false), left(false), right(false), front(false), back(false) {}
//
//RaycastOrigins3D::RaycastOrigins3D() : fwd(0.f), rear(0.f), bottom(0.f), top(0.f), front(0.f), back(0.f) {}
//
//Controller3D::Controller3D(const pybind11::kwargs& kwargs) : Controller(kwargs), _maskPlatform(2) {
//	m_skinWidth = py_get_dict<float>(kwargs, "skinWidth", .015f);
//	_maskPlatform = py_get_dict<int>(kwargs, "mask_platform", 2);
//}
//
//std::shared_ptr<Model> Controller3D::getDebugModel() {
//	auto shape = std::make_shared<AABB3D>(
//			-m_center.x, -m_center.x + m_size.x,
//			-m_center.y, -m_center.y + m_size.y,
//			-m_center.z, -m_center.z + m_size.z);
//	auto& m = ModelMaker::instance();
//	auto model = m.make(shape, glm::vec4(1.f, 0.f, 0.f, 1.f), FillType::OUTLINE);
//	return model;
//}
//
//void Controller3D::updateRaycastOrigins() {
//	auto worldMatrix = m_node->getWorldMatrix();
//
//	auto maxp = worldMatrix * glm::vec4(m_size - m_center, 1.0f);
//	auto minp = worldMatrix * glm::vec4(m_center, 1.0f);
//
//	m_raycastOrigins.fwd = maxp.x;
//	m_raycastOrigins.rear = minp.x;
//
//	m_raycastOrigins.bottom = minp.y;
//	m_raycastOrigins.top = maxp.y;
//
//	m_raycastOrigins.back = minp.z;
//	m_raycastOrigins.front = maxp.z;
//}
//
//void Controller3D::move(glm::vec3& delta, bool) {
//	if (delta == glm::vec3(0.0f)) return;
//	//auto scale = m_entity->getScaleVec();
//
//	updateRaycastOrigins();
//	m_details.Reset();
//	//m_ppp.clear();
//
//	bool movingHorizontally = !isEqual(delta.x, 0.0f) || !isEqual(delta.z, 0.0f);
//	if (movingHorizontally)
//		horizontalCollisions(delta);
//		if (!isEqual(delta.y, 0.0f))
//			verticalCollisions(delta);
//		glm::vec3 actualMove(delta); //(dx.x / scale.x, dx.y/scale.y, dx.z/scale.z);
//		m_node->move(actualMove);
//	}
//
//void Controller3D::verticalCollisions(glm::vec3 &velocity) {
//	float directionY = sign(velocity.y);
//	float rayLength = std::abs(velocity.y) + m_skinWidth;
//	Node* obstacle = nullptr;
//
//	float velx = velocity.x * (m_node->getFlipX() ? -1.0f : 1.0f);
//	float velz = velocity.z;
//	glm::vec3 pos = m_node->getWorldPosition();
//	//vec2 r0 = pos + vec2(-m_halfSize[0], directionY > 0 ? m_halfSize[1] : -m_halfSize[1]);
//	//auto pos = m_entity->GetPosition();
//	//auto scale = m_entity->GetScale();
//	//float dy = scale * (m_shift.y + (directionY > 0 ? m_halfSize[1] : -m_halfSize[1]));
//	std::array<glm::vec3, 4> raycastOrigins = {
//			glm::vec3(m_raycastOrigins.fwd + velx, m_raycastOrigins.bottom, m_raycastOrigins.front + velz),
//			glm::vec3(m_raycastOrigins.rear + velx, m_raycastOrigins.bottom,m_raycastOrigins.front + velz),
//			glm::vec3(m_raycastOrigins.fwd + velx, m_raycastOrigins.bottom, m_raycastOrigins.back + velz),
//			glm::vec3(m_raycastOrigins.rear + velx, m_raycastOrigins.bottom,m_raycastOrigins.back + velz)};
//
//	for (const auto& r0 : raycastOrigins) {
//		//int collMask = (directionY == -1 ? (m_maskDown) : m_maskUp);
//		RayCastHit hit = m_collisionEngine->rayCast(r0, glm::vec3(0, directionY, 0), rayLength, _maskPlatform);
//		if (hit.collide) {
//			velocity.y = (hit.length - m_skinWidth) * directionY;
//			rayLength = hit.length;
//			m_details.below = directionY == -1;
//			m_details.above = directionY == 1;
//			obstacle = hit.entity->getNode();
//		}
//	}
//
//
//}
//
//
//
//// this will have x and z components
//void Controller3D::horizontalCollisions(glm::vec3& vel) {
//	bool goingForward = vel.x > 0.f;
//
//	bool flipx = m_node->getFlipX();
//	if (goingForward) {
//		float rayLength = vel.x + m_skinWidth;
//		float dir_x = flipx ? -1.0 : 1.0;
//
//		std::array<glm::vec3, 2> raycastOrigins = {
//				glm::vec3(m_raycastOrigins.fwd, m_raycastOrigins.bottom, m_raycastOrigins.front),
//				glm::vec3(m_raycastOrigins.fwd, m_raycastOrigins.bottom, m_raycastOrigins.back)};
//
//		for (const auto &r0 : raycastOrigins) {
//			RayCastHit hit = m_collisionEngine->rayCast(r0, glm::vec3(dir_x, 0.f, 0.f), rayLength, _maskPlatform);
//			if (hit.collide) {
//				vel.x = std::max(hit.length - m_skinWidth, 0.0f) * sign(vel.x);
//				rayLength = hit.length;
//				m_details.left = dir_x == -1.f;
//				m_details.right = dir_x == 1.f;
//			}
//		}
//	}
//
//	if (fabs(vel.z) > 0.0f) {
//		float z = vel.z > 0 ? m_raycastOrigins.front : m_raycastOrigins.back;
//		float directionZ = vel.z < 0 ? -1.0 : 1.0;
//		glm::vec3 dirZ (0.0f, 0.0f, vel.z > 0 ? 1.0f : -1.0f);
//		float dx = vel.x * (flipx ? -1.0f : 1.0f);
//		std::array<glm::vec3, 2> raycastOrigins = {
//				glm::vec3(m_raycastOrigins.fwd + dx, m_raycastOrigins.bottom, z),
//				glm::vec3(m_raycastOrigins.back + dx, m_raycastOrigins.bottom, z)};
//		float rayLength = fabs(vel.z) + m_skinWidth;
//		for (const auto &r0 : raycastOrigins) {
//			RayCastHit hit = m_collisionEngine->rayCast(r0, glm::vec3(0.0f, 0.0f, 1.0f) * directionZ, rayLength, _maskPlatform);
//			if (hit.collide) {
//				vel.z = std::max(hit.length - m_skinWidth, 0.0f) * sign(vel.z);
//				rayLength = hit.length;
//				m_details.back = directionZ == -1;
//				m_details.front = directionZ == 1;
//			}
//		}
//	}
//}
//bool Controller3D::isFalling(float dir) {
//	return false;
//}