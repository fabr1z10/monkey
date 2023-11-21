#pragma once

#include "../component.h"
#include <glm/glm.hpp>
#include "../runners/collision_engine.h"
#include "../model.h"
#include <pybind11/pybind11.h>



/// a controller is a class that checks if node movements are valid
class Controller : public Component {
public:
	Controller(const pybind11::kwargs&);
	void start() override;

	/// tests whether collider is colliding below
	bool grounded() const;
	void setGrounded(bool);

	/// tests whether collider is colliding above
	bool ceiling() const;
	void setCeiling();

	/// tests whether collider is colliding right
	bool right() const;
	void setRight();

	/// tests whether collider is colliding left
	bool left() const;
	void setLeft();

	/// this method updates the origin points of the rays that are casted to
	/// constraint movement. Obviously, the points depend whether the player is 2D or 3D
	/// in the former case, you have only right, left, up and down. With 3D, you have
	/// to add front and back (along z axis)
	virtual void updateRaycastOrigins() = 0;
	virtual bool isFalling(float) = 0;

	void setSize(glm::vec3 size, glm::vec3 offset);
	virtual void move(glm::vec3&, bool forced);
	glm::vec3 getSize() const;
	glm::ivec2 getMask() const;
	void setMask(int, int);
	using Base = Controller;
	virtual void resetCollisions();
protected:
	void setDebugShape();
	virtual std::shared_ptr<Model> getDebugModel() = 0;
	Node* m_debugShape;
	void computeCoordinates();
	glm::vec3 m_size;
	glm::vec3 m_center;
	bool m_debug;
	ICollisionEngine* m_collisionEngine;
	glm::vec3 m_localTopFwd;
	glm::vec3 m_localTopBack;
	glm::vec3 m_localBottomFwd;
	glm::vec3 m_localBottomBack;
    std::string _batchId;
	int m_maskUp;
	int m_maskDown;
	// bit 0 = below, 1 = above, 2 = right, 3 = left
	unsigned m_collisionStatus;
};

inline bool Controller::grounded() const {
	return (m_collisionStatus & 0x01u) != 0;
}

inline bool Controller::ceiling() const {
	return (m_collisionStatus & 0x02u) != 0;
}

inline bool Controller::right() const {
	return (m_collisionStatus & 0x04u) != 0;
}

inline bool Controller::left() const {
	return (m_collisionStatus & 0x08u) != 0;
}


inline void Controller::setGrounded(bool value) {
	if (value)
		m_collisionStatus |= 0x01;
	else
		m_collisionStatus &= 0xFE;
	//m_details.below = value;
}

inline void Controller::setLeft() {
	m_collisionStatus |= 0x08u;
}

inline void Controller::setRight() {
	m_collisionStatus |= 0x04u;
}

inline void Controller::setCeiling() {
	m_collisionStatus |= 0x02u;
}