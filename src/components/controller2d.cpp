#include "controller2d.h"

#include <utility>
#include "../shapes/aabb.h"
#include "../models/modelmake.h"
#include "../pyhelper.h"
#include "../node.h"
#include "../util.h"
#include "../engine.h"
#include "mover.h"
#include "../error.h"
#include <GLFW/glfw3.h>

using namespace shapes;

extern GLFWwindow * window;

using namespace pybind11::literals; // to bring in the `_a` literal


Controller2D::Controller2D(const pybind11::kwargs& kwargs) : Controller(kwargs), _mover(nullptr), _state(-1) {
    // this cannot rotate!!!
	m_maxClimbAngle = glm::radians(py_get_dict<float>(kwargs, "max_climb_angle", 80.0f));
	m_maxDescendAngle = glm::radians(py_get_dict<float>(kwargs, "max_descend_angle", 80.0f));
	m_skinWidth = py_get_dict<float>(kwargs, "skinWidth", .015f);
	m_horizontalRayCount = py_get_dict<int>(kwargs, "horizontal_ray_count", 4);
	m_verticalRayCount = py_get_dict<int>(kwargs, "vertical_ray_count", 4);
	m_maskDown = py_get_dict<int>(kwargs, "mask_down", 2 | 32);
	m_maskUp = py_get_dict<int>(kwargs, "mask_up", 2);
	m_platforms = nullptr;
	//_gravity = py_get_dict<float>(kwargs, "gravity", 0.f);
	_acc = py_get_dict<float>(kwargs, "acceleration", 1.f);
    _maxSpeed = py_get_dict<float>(kwargs, "speed", 10.f);
    _jumpHeight = py_get_dict<float>(kwargs, "jump_height", 16.f);
	_timeToJumpApex = py_get_dict<float>(kwargs, "time_to_jump_apex", 0.5);
	// gravity and jump velocity are computed from jump height and time to jump apex
	_gravity = (2.0f * _jumpHeight) / (_timeToJumpApex * _timeToJumpApex);
	_jumpVelocity = (2.0f * _jumpHeight) / _timeToJumpApex;

//	_maskUp = 2;
//	_maskDown = 2 | 32;
	_foeFlag = 1;
}

void Controller2D::start() {
	Controller::start();
	for (auto& m : _controllers) {
		m->init(m_node);
	}
}

void Controller2D::shutdown() {
	_controllers.clear();
}


int Controller2D::addState(pybind11::object obj) {
	_pythonObj.push_back(obj);
	auto id = _controllers.size();
	_controllers.emplace_back(obj.cast<std::shared_ptr<ControllerState>>());
	//state->init(m_node);
	return id;
}

void Controller2D::update(double dt) {
	if (_state < 0) {
		return;
	}

	_controllers[_state]->update(dt);
}


PlayerController2D::PlayerController2D(const std::string& batch, const pybind11::kwargs &args) : Controller2D(args), _currentModel(-1), _batch(batch) {

	_controllers.push_back(std::make_shared<PlayerControllerState>());
	//#StateInfo info;
	//info.update = [&] (double dt) { this->defaultController(dt); };
	//_controllers.emplace_back(info);

	setState(0);
//	_walk = py_get_dict<std::string>(args, "walk", "walk");
//	_idle = py_get_dict<std::string>(args, "idle", "idle");
//	_slide = py_get_dict<std::string>(args, "slide", "slide");
//	_jumpUp = py_get_dict<std::string>(args, "jumpUp", "jump");
//	_jumpDown = py_get_dict<std::string>(args, "jumpDown", "jump");
}

void PlayerControllerState::init(Node * node) {
	ControllerState::init(node);
	_controller = dynamic_cast<PlayerController2D*>(node->getComponent<Controller>());
}

void PlayerControllerState::update(double dt) {

    auto dtf = static_cast<float>(dt);

    if (_controller->grounded()) {
        _velocity.y = 0.f;
    }

    glm::vec2 acceleration = glm::vec2(0.f, -_controller->getGravity());

    bool left = glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS;
    bool right = glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS;
    if (!left && !right) {
        _direction = 0;
    } else if (left && !right) {
    	if (!_node->getFlipX()) {
    		_velocity.x *= -1.f;
    	}
        _direction = -1;
		_node->setFlipX(true);
    } else {
		if (_node->getFlipX()) {
			_velocity.x *= -1.f;
		}
        _direction = 1;
        _node->setFlipX(false);
    }

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS && _controller->grounded()) {
        _velocity.y = _controller->getJumpVelocity();
    }





	if (_direction != 0) {
	    // x-acceleration should be always positive unless when !fliph && dir==-1
        acceleration.x = _controller->getAcceleration();
	} else {
		// apply deceleration only if velocity above threshold
		if (fabs(_velocity.x) > (_controller->getAcceleration() * dtf) + 0.1f) {
		    // decel, acceleration should be opposite velocity
			acceleration.x = -signf(_velocity.x) * _controller->getAcceleration();
		} else {
			acceleration.x = 0.0f;
            _velocity.x = 0.0f;
		}
	}

    _velocity += acceleration * dtf;

	auto ms = _controller->getMaxSpeed();
    if (fabs(_velocity.x) > ms) {
        _velocity.x = signf(_velocity.x) * ms;
	}


    glm::vec2 delta = _velocity * dtf;

    _controller->move(delta, false);

	if (_controller->right() || _controller->left()) {
		_velocity.x = 0.f;
	}

	// handle animation
	const auto& mi = _controller->getModelInfo();
	if (_controller->grounded()) {
		if (_velocity.x > 0) {
			_node->setAnimation(mi.walk);
		} else if (_velocity.x == 0.f) {
			_node->setAnimation(mi.idle);
		} else if (_velocity.x < 0) {
			_node->setAnimation(mi.slide);
		}
	} else {
		if (delta.y > 0) {
			_node->setAnimation(mi.jumpUp);
		} else {
			_node->setAnimation(mi.jumpDown);
		}

	}

}



Controller2D::~Controller2D() {
	if (m_platforms != nullptr) {
		m_platforms->unregisterComponent(this);
		m_platforms->forceRemove(this);
	}
}

//std::shared_ptr<Model> Controller2D::getDebugModel() {
//	auto shape = std::make_shared<AABB>(-m_center.x, -m_center.x + m_size.x, -m_center.y, -m_center.y + m_size.y);
//	auto& m = ModelMaker::instance();
//    auto& engine = Engine::instance();
//
//    auto model = m.make(engine.getColliderOutlineBatch(), shape, glm::vec4(1.f, 0.f, 0.f, 1.f), FillType::OUTLINE);
//	return model;
//}

void Controller2D::updateRaycastOrigins() {
    glm::vec2 pos = m_node->getWorldPosition();
    float fx = m_node->getFlipX() ? -1.f: 1.f;

	m_raycastOrigins.topFwd = pos + glm::vec2(m_localTopFwd.x * fx, m_localTopFwd.y);
	m_raycastOrigins.topBack = pos + glm::vec2(m_localTopBack.x * fx, m_localTopBack.y);
	m_raycastOrigins.bottomFwd = pos + glm::vec2(m_localBottomFwd.x * fx, m_localBottomFwd.y);
	m_raycastOrigins.bottomBack = pos + glm::vec2(m_localBottomBack.x * fx, m_localBottomBack.y);
	float width = fabs(m_raycastOrigins.topFwd.x - m_raycastOrigins.topBack.x);
	float height = m_raycastOrigins.topFwd.y - m_raycastOrigins.bottomFwd.y;
	m_horizontalRaySpacing = height / (m_horizontalRayCount - 1.f);
	m_verticalRaySpacing = width / (m_verticalRayCount - 1.f);
	//std::cout << "bottom fwd/back " << m_raycastOrigins.bottomFwd.x << "; " << m_raycastOrigins.bottomBack.x << "\n";
}

void Controller2D::move(glm::vec2& delta, bool forced) {

	if (delta == glm::vec2(0.0f)) return;

	updateRaycastOrigins();

	// check if character was grounded at last iteration
	m_wasGnd = grounded();
	//m_details.reset();
	resetCollisions();
	if (forced) {
		/// ? not sure about this
		setGrounded(true);
	}

	if (delta.y < 0 && m_wasGnd) {
		descendSlope(delta);
	}
	if (!isEqual(delta.x, 0.0f))
		horizontalCollisions(delta);
	if (!isEqual(delta.y, 0.0f))
		verticalCollisions(delta, forced);

	//m_node->move(glm::translate(delta));
	m_node->move(glm::vec3(delta, 0.f));
    //std::cout << "y = " << m_node->getWorldPosition().y << "\n";

	// after movement, I check collisions
	if (delta != glm::vec2(0.f) &&  _collider != nullptr) {
		auto collisionReport = m_collisionEngine->shapeCast(_collider, delta);
	}
}


void Controller2D::resetCollisions() {
	Controller::resetCollisions();
	m_climbingSlope = false;
	m_descendingSlope = false;
	m_slopeAngleOld = m_slopeAngle;
	m_slopeAngle = 0.0f;
}

// in general v.x > 0 means foing forward, v.x < 0 going back
// then we have the following table:
//  v.x > 0   flip        going
// -----------------------------------
//    T       F           right
//    T       T			  left
//    F       F           left
//    F       T			  right
// so we can say that we go right if v.x > 0 != flip and viceversa.
void Controller2D::descendSlope(glm::vec2& velocity) {
	//std::cout << "calling descendslope with " << velocity.x << "\n";
	if (velocity.x == 0.0f) return;
	bool goingForward = velocity.x > 0.0f;
	//auto directionX = signf(velocity.x);
	float dir_x = (goingForward == m_node->getFlipX()) ? -1.f : 1.f;

	auto r0 = dir_x > 0.f ? m_raycastOrigins.bottomFwd : m_raycastOrigins.bottomBack;
	RayCastHit hit = m_collisionEngine->rayCast(r0, Direction::Y, -100.0f, m_maskDown, m_node);
	//std::cout << "hit = " << hit.collide<<"\n";
	if (hit.collide) {
		float slopeAngle = angle(hit.normal, glm::vec2(0.f, 1.f));
		if (slopeAngle != 0 && slopeAngle <= m_maxDescendAngle) {
			if (signf(hit.normal.x) == dir_x) {
				if (hit.length - m_skinWidth <= tan(slopeAngle) * fabs(velocity.x)) {
					float moveDistance = fabs(velocity.x);
					float descendVelocityY = sin(slopeAngle) * moveDistance;
					velocity.x = signf(velocity.x) *cos(slopeAngle) * moveDistance;// * sign(velocity.x);
					velocity.y -= descendVelocityY;
					m_slopeAngle = slopeAngle;
					m_descendingSlope = true;
					setGrounded(true);
				}
			}
		}
	}
}

void Controller2D::horizontalCollisions(glm::vec2& velocity) {
	float directionX = signf(velocity.x);
	bool goingForward = directionX > 0.f;
	float rayLength = fabs(velocity.x) + m_skinWidth;
	float dir_x = (goingForward == m_node->getFlipX()) ? -1.f : 1.f;
    auto r0 = directionX > 0.f ? m_raycastOrigins.bottomFwd : m_raycastOrigins.bottomBack;
	for (int i = 0; i < m_horizontalRayCount; i++) {
		auto rayOrigin = r0 + glm::vec2(0.0f, i * m_horizontalRaySpacing);
		RayCastHit hit = m_collisionEngine->rayCast(rayOrigin, Direction::X, rayLength * dir_x, m_maskDown, m_node);
		if (hit.collide) {
			int colliderFlag = hit.entity->getCollisionFlag();
			if (colliderFlag == _foeFlag) {
				// call responde by passing the direction ... so for instance you can know
				// the directin from which the enemy is hit
				// handle
			} else /*if ((colliderFlag & _platformFlag)!= 0)*/ {
				float slopeAngle = angle(hit.normal, glm::vec3(0.f, 1.f, 0.f));
				if (i == 0 && slopeAngle <= m_maxClimbAngle) {
					if (m_descendingSlope) {
						m_descendingSlope = false;
						velocity = m_velocityOld;
					}
					float distanceToSlopeStart = 0;
					if (slopeAngle != m_slopeAngleOld) {
						distanceToSlopeStart = hit.length - m_skinWidth;
						velocity.x -= distanceToSlopeStart * directionX;
					}
					climbSlope(velocity, slopeAngle);
					velocity.x += distanceToSlopeStart * directionX;
				}
				if (!m_climbingSlope || slopeAngle > m_maxClimbAngle) {
					velocity.x = std::max(hit.length - m_skinWidth, 0.0f) * directionX;
					rayLength = hit.length;
					if (m_climbingSlope) {
						velocity.y = tan(m_slopeAngle)* fabs(velocity.x);
					}
					bool faceRight = !m_node->getFlipX();
					auto dir_x = (goingForward == m_node->getFlipX()) ? -1 : 1;
					if (dir_x == -1) {
						setLeft();
					} else {
						setRight();
					}
				}
			}
		}
	}
}



void Controller2D::verticalCollisions(glm::vec2& velocity, bool forced) {
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
	glm::vec2 r0 = directionY > 0.f ? (faceRight ? m_raycastOrigins.topBack : m_raycastOrigins.topFwd) :
				   (faceRight ? m_raycastOrigins.bottomBack : m_raycastOrigins.bottomFwd);
	for (int i = 0; i < m_verticalRayCount; i++) {
		auto rayOrigin = r0 + glm::vec2(i * m_verticalRaySpacing + dir_x * velocity.x, 0.f) ;
		int collMask = (directionY == -1 ? (m_maskDown) : m_maskUp);
		RayCastHit hit = m_collisionEngine->rayCast(rayOrigin, Direction::Y, directionY * rayLength, collMask, m_node);
		bool ciao = grounded();
		if (hit.collide) {
			//if (!m_wasGnd) {
			//}
			//std::cout << "raylen = " << rayLength << "; dist = " << hit.length << "\n";
			atleast = true;
			velocity.y = (hit.length - m_skinWidth) * directionY;
			rayLength = hit.length;
			if (m_climbingSlope) {
				velocity.x = (velocity.y / tan(m_slopeAngle)) * signf(velocity.x);
			}
			if (directionY == -1) {
				setGrounded(true);
			} else {
				setCeiling();
			}
//			m_details.below = directionY == -1;
//			m_details.above = directionY == 1;
			//if (directionY < 0.0f) {
				if (hit.length < obstacleDistance) {
					obstacle = hit.entity->getNode();
					obstacleDistance = hit.length;
				}
				//obstacles.insert(hit.entity->getNode());
			//}
		}
	}


	// register to new platforms -- needed for moving platforms+
	if (obstacle != nullptr) {
		//auto* platformController = obstacle->getComponent<Platform>();
		auto* mover = obstacle->getComponent<Mover>();

		if (mover != nullptr && directionY < 0.f) {
			mover->addLinkedNode(this->getNode());
			_mover = mover;
//            if (platformController != m_platforms) {
//                if (platformController != nullptr) {
//                    platformController->registerComponent(this);
//                }
//                if (m_platforms != nullptr) {
//                    m_platforms->unregisterComponent(this);
//                }
//                m_platforms = platformController;
//            }
        }
	} else {
		if (_mover != nullptr) {
			_mover->rmLinkedNode(this->getNode());
			_mover = nullptr;
		}
	}

}

void Controller2D::setPlatform(Platform * p) {
	setGrounded(true);
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


void Controller2D::climbSlope(glm::vec2& velocity, float slopeAngle) {
	float moveDistance = fabs(velocity.x);
	float climbVelocityY = sin(slopeAngle) * moveDistance;
	if (velocity.y <= climbVelocityY) {
		velocity.y = climbVelocityY;
		velocity.x = cos(slopeAngle) * moveDistance * signf(velocity.x);
		setGrounded(true);
		//m_details.below = true;
		m_climbingSlope = true;
		m_slopeAngle = slopeAngle;
	}
}

bool Controller2D::isFalling(float dir) {
	// just casting a ray downward from fwd point
 	updateRaycastOrigins();

	auto rayOrigin = (dir > 0) ?
	        m_raycastOrigins.bottomFwd :
	        m_raycastOrigins.bottomBack;//(dir < 0.f ? m_raycastOrigins.bottomLeft : m_raycastOrigins.bottomRight);
	//rayOrigin.x += (dir<0 ? -1.f : 1.f) * 8.f;
	//glm::vec2 rayOrigin = (dir == -1) ? m_raycastOrigins.bottomLeft : m_raycastOrigins.bottomRight;
	RayCastHit hit = m_collisionEngine->rayCast(rayOrigin, Direction::Y, -0.5f, m_maskDown, m_node);
	if (!hit.collide) {
		return true;
	}
	return false;
}

void Controller2D::resetDetails() {
	resetCollisions();
}

void PlayerController2D::start() {
	Controller2D::start();

}

void PlayerController2D::setModel(int index) {
	if (index != _currentModel) {
		if (index >= _models.size()) {
			GLIB_FAIL("PlayerController2D - don't have model " << index);
		}
		m_node->setModel(_models[index].model, py::dict("batch"_a = _batch));
		_currentModel = index;
	}
}
void PlayerController2D::addModel(std::shared_ptr<Model> model, const std::string &idleAnimation,
								  const std::string &walkAnimation, const std::string &slideAnimation,
								  const std::string &jumpUpAnimation, const std::string &jumpDownAnimation) {
	ModelInfo mi;
	mi.model = std::move(model);
	mi.idle = idleAnimation;
	mi.walk = walkAnimation;
	mi.slide = slideAnimation;
	mi.jumpUp = jumpUpAnimation;
	mi.jumpDown = jumpDownAnimation;
	_models.push_back(mi);

}

void PlayerController2D::addKeyCallback(int id, pybind11::function f) {
	_callbacks[id] = 	f;
}

void PlayerController2D::setNode(Node * node) {
	Controller2D::setNode(node);
	setModel(0);
}

int PlayerController2D::keyCallback(GLFWwindow *, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		auto it = _callbacks.find(key);
		if (it != _callbacks.end()) {
			(*it->second)();
		}

	}
}