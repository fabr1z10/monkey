#include "mover.h"
#include "../node.h"
#include "../engine.h"
#include "../runners/collision_engine.h"


Mover::Mover(const pybind11::kwargs& args) : Component(args), _currentAction(0), _collider(nullptr), _collisionEngine(nullptr) {

}

void Mover::addMove(std::shared_ptr<NodeAction> action) {
	_movements.push_back(action);
}

void Mover::addLinkedNode(Node * n) {
	_linkedNodes.insert(n);
}

void Mover::rmLinkedNode(Node * n) {
	_linkedNodes.erase(n);
}

void Mover::start() {
	// check if I have a collider

	if (_movements.empty()) {
		return;
	}
	_collider = m_node->getComponent<Collider>();
	if (_collider != nullptr) {
		auto& engine = Engine::instance();
		auto room = engine.getRoom();
		_collisionEngine = room->getRunner<ICollisionEngine>();
	}
	_currentAction = 0;
	for (auto& m : _movements) m->setNode(m_node);
	_movements[_currentAction]->reset();
}

void Mover::update(double dt) {
	if (_movements.empty()) {
		return;
	}
	auto initialPos = m_node->getWorldPosition();
	int exitValue = _movements[_currentAction]->run(dt);
	auto endPos = m_node->getWorldPosition();
	auto delta = endPos - initialPos;
	for (const auto& l : _linkedNodes) {
		l->movea(delta);
	}

	// after movement, I check collisions
	if (_collider != nullptr) {
		auto collisionReport = _collisionEngine->shapeCast(_collider);
		if (!collisionReport.empty()) {
			std::cout << "COLLISION\n";
			for (auto& c : collisionReport) {
				c.entity->getNode()->movea(-c.report.direction * c.report.distance * 1.05f);
				if (c.report.direction.y < 0) {
					_linkedNodes.insert(c.entity->getNode());
				}

			}
		}
	}

	if (exitValue == 0) {
		_currentAction ++;
		if (_currentAction >= _movements.size()) {
			_currentAction = 0;
		}
		_movements[_currentAction]->reset();
	}
}