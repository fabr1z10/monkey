#include <iostream>
#include "platform.h"
#include "controller2d.h"
#include "../node.h"
#include "../engine.h"
#include "../util.h"

Platform::~Platform() {
	for (const auto& c : m_characters) {
		c->resetPlatform();
	}
}

void Platform::start() {
	auto& engine = Engine::instance();
	auto room = engine.getRoom();
	m_collisionEngine = room->getRunner<CollisionEngine>();
	m_node->onMove.reg([&](Node* node) { move(node); });
	m_lastPosition = m_node->getWorldPosition();
	auto coll = m_node->getComponent<Collider>();
	m_platformWidth = m_node->getComponent<Collider>()->getStaticBounds().getSize().x;
}

void Platform::setActive(bool value) {
	Component::setActive(value);
	if (!value) {
		unregisterAll();
	}
}

void Platform::unregisterComponent(Controller2D* character) {
	//m_characters.erase(character);
	m_removeBuffer.push_back(character);
	character->getNode()->onRemove.unreg(m_pippo.at(character));
	//m_characters.erase(character);
}

void Platform::forceRemove(Controller2D * c) {
	m_characters.erase(c);
}

void Platform::registerComponent(Controller2D* character) {
	m_characters.insert(character);
	int a = character->getNode()->onRemove.reg([&, character] (Node* node) {
		m_characters.erase(character);
	});
	m_pippo[character] = a;

}

void Platform::unregisterAll() {
	m_characters.clear();
}



void Platform::move(Node* node) {
	for (const auto& b : m_removeBuffer) {
		m_characters.erase(b);
	}
	m_removeBuffer.clear();
	glm::vec3 currentPosition = node->getWorldPosition();
	glm::vec3 delta = currentPosition - m_lastPosition;
	if (delta.x != 0.0f || delta.y != 0.0f) {

		// check if I capture any new characters
		Bounds b(currentPosition);
		b.addPoint(currentPosition + glm::vec3(m_platformWidth, 0.f, 0.f) + delta);
		auto location = m_collisionEngine->getLocation(b);
		float ldelta = glm::length(delta);
		for (auto i = location.first.x; i <= location.second.x; ++i) {
			for (auto j = location.first.y; j <= location.second.y; ++j) {
				const auto* cs = m_collisionEngine->getColliders(glm::ivec3(i, j, 0));
				if (cs != nullptr) {
					for (const auto &c : cs->colliders) {
						if (c->getCollisionFlag() == 1) {
							auto cpos1 = c->getNode()->getWorldPosition();
							auto pos = cpos1 - m_lastPosition;
							// the character is picked up by the platform if its position is contained
							// in the parallelogram formed by (1, 0) and delta.
							float lambda = 0, mu = 0;
							if (solve2x2(m_platformWidth, delta.x, pos.x, 0.0f, delta.y, pos.y, lambda, mu) == 0 &&
								lambda >= 0.f && lambda <= 1.f && mu >= 0.f && mu <= 1.f) {
								auto *controller2D = dynamic_cast<Controller2D *>(c->getNode()->getComponent<Controller>());
								if (controller2D != nullptr && m_characters.count(controller2D) == 0) {
									registerComponent(controller2D);
									//m_characters.insert(controller2D);
									controller2D->setPlatform(this);
								}
							}
						}
					}
				}

			}
		}



		for (const auto& c : m_characters) {
			// TODO Problem: What happens if moving the character send it to another platform?
			// this would trigger the unregister and so the vector will change while iterating
			if (c->getNode()->getFlipX()) {
				delta.x *= -1.f;
			}
			c->move(delta, true);

		}
	}
	m_lastPosition = currentPosition;
}