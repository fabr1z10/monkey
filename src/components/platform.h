#pragma once

#include "../component.h"
#include <unordered_set>
#include "glm/glm.hpp"
#include "../runners/collision_engine.h"

class Controller2D;

class Platform : public Component {
public:
	Platform() {}
	virtual ~Platform();
	void start() override;
	// void update(double) override;
	void registerComponent(Controller2D*);
	void unregisterComponent(Controller2D*);
	void forceRemove(Controller2D*);
	void unregisterAll();
	void setActive(bool) override;
	void move(Node*);
	using Base = Platform;
private:
	std::unordered_set<Controller2D*> m_characters;
	std::vector<Controller2D*> m_removeBuffer;
	glm::vec3 m_lastPosition;
	CollisionEngine* m_collisionEngine;
	float m_platformWidth;
	std::unordered_map<Controller2D*, int> m_pippo;
};