#include "controller_state.h"
#include "../node.h"

void ControllerState::init(Node * node) {
	_node = node;
}

void PyControllerState::init(Node * node) {
	_node = node;
	PYBIND11_OVERRIDE(
			void,               				// Return type
			ControllerState,	               	// Parent class
			init, node    		    				// Name of function in C++
	);

}

void PyControllerState::start() {

	PYBIND11_OVERRIDE(
			void,               				// Return type
			ControllerState,	               	// Parent class
			start    		    				// Name of function in C++
	);

}

void PyControllerState::update(double dt) {

	PYBIND11_OVERRIDE(
			void,               			// Return type
			ControllerState,               	// Parent class
			update,		    				// Name of function in C++
			dt
	);

}