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
	virtual bool grounded() const = 0;
	void setSize(glm::vec3 size, glm::vec3 offset);
	virtual void move(glm::vec3&, bool forced);
	glm::vec3 getSize() const;
	using Base = Controller;
protected:
	void setDebugShape();
	virtual std::shared_ptr<Model> getDebugModel() = 0;
	Node* m_debugShape;
	void computeCoordinates();
	glm::vec3 m_size;
	glm::vec3 m_center;
	bool m_debug;
	CollisionEngine* m_collisionEngine;
	glm::vec3 m_localTopFwd;
	glm::vec3 m_localTopBack;
	glm::vec3 m_localBottomFwd;
	glm::vec3 m_localBottomBack;
};