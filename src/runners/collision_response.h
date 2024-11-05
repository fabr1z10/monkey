#pragma once

#include <pybind11/pybind11.h>
#include "../components/collider.h"
#include "../hashpair.h"
#include "../pycast.h"





class CollisionResponse {
public:
	CollisionResponse(int tag1, int tag2) : _tag1(tag1), _tag2(tag2) {}
	virtual ~CollisionResponse() = default;
	virtual void onStart(Collider* first, Collider* second, glm::vec2 move, int whoMoves) const;

	virtual void onEnd(Collider* first, Collider* second) const;


	int getTag1() const;
	int getTag2() const;
protected:
	int _tag1;
	int _tag2;
};

inline int CollisionResponse::getTag1() const {
	return _tag1;
}
inline int CollisionResponse::getTag2() const {
	return _tag2;
}




class PyCollisionResponse : public CollisionResponse {
public:
	using CollisionResponse::CollisionResponse;  // Inherit constructors

	void onStart(Collider* first, Collider* second, glm::vec2 move, int whoMoves) const override;

	void onEnd(Collider* first, Collider* second) const override;

};

//class CollisionResponseManager {
//public:
//    bool hasCollision(Collider*, Collider*);
//    bool hasCollision(int, int);
//    void add(int, int, const pybind11::kwargs&);
//    void onStart(Collider*, Collider*, glm::vec3);
//    void onStay(Collider*, Collider*);
//    void onEnd(Collider*, Collider*);
//    void onStart(Node* node1, Node* node2, int tag1, int tag2, glm::vec3);
//
//private:
//    std::unordered_map<std::pair<int, int>, CollisionResponse> m_response;
//
//};

