#pragma once

#include "../component.h"
#include "../actions/move.h"

class Collider;
class ICollisionEngine;

class Mover : public Component {


public:
	Mover(const pybind11::kwargs&);
	void addMove(std::shared_ptr<NodeAction>);
	void clear();
	void start() override;
	void update(double) override;
	void addLinkedNode(Node*);
	void rmLinkedNode(Node*);
	using Base = Mover;
private:
	int _currentAction;
	std::list<std::shared_ptr<NodeAction>> _movements;
	Collider* _collider;
	ICollisionEngine* _collisionEngine;
	// these nodes get moved along with the mover
	std::unordered_set<Node*> _linkedNodes;


};