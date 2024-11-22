#pragma once

#include <pybind11/pybind11.h>

class Node;


class ControllerState {
public:
	ControllerState() = default;

	virtual void init(Node*);

	virtual ~ControllerState() = default;

	virtual void start() {}

	virtual void update(double) {}

	Node* getNode();
protected:
	Node* _node;
};

inline Node * ControllerState::getNode() {
	return _node;
}






class PyControllerState : public ControllerState {
public:
	using ControllerState::ControllerState;  // Inherit constructors

	void init(Node*) override;

	void start() override;

	void update(double) override;

};

