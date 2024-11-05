#include "collision_response.h"
#include <iostream>

void CollisionResponse::onStart(Collider* first, Collider* second, glm::vec2 move, int whoMoves) const {
	std::cout << "base called\n";
}

void CollisionResponse::onEnd(Collider* first, Collider* second) const {
	std::cout << "onEnd base called\n";
}


void PyCollisionResponse::onStart(Collider* first, Collider* second, glm::vec2 move, int whoMoves) const {
	if (first->getCollisionTag() != _tag1) {
		std::swap(first, second);
		if (whoMoves == 0) {
			whoMoves = 1;
		} else if (whoMoves == 1) {
			whoMoves = 0;
		}
	}

	PYBIND11_OVERRIDE(
		void,               				// Return type
		CollisionResponse,               	// Parent class
		onStart,	    					// Name of function in C++
		first, second, move, whoMoves
	);

}

void PyCollisionResponse::onEnd(Collider* first, Collider* second) const {
	if (first->getCollisionTag() != _tag1) {
		std::swap(first, second);
	}

	PYBIND11_OVERRIDE(
			void,               				// Return type
			CollisionResponse,               	// Parent class
			onEnd,	    					// Name of function in C++
			first, second
	);

}