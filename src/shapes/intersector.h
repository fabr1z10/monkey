#pragma once

#include "../runners/collision.h"
#include "../shape.h"
#include <unordered_map>
#include <functional>
#include <utility>
#include "../hashpair.h"


class Intersector {
public:
    typedef CollisionReport(*methodPtr)(shapes::Shape*, shapes::Shape*, const glm::mat4&, const glm::mat4&);

    Intersector() = default;

    void add(shapes::ShapeType t1, shapes::ShapeType t2, methodPtr ptr) ;

    CollisionReport intersect(shapes::Shape * s1, shapes::Shape * s2, const glm::mat4 & t1, const glm::mat4 & t2);



protected:
    std::unordered_map<std::pair<shapes::ShapeType, shapes::ShapeType>, methodPtr> _map;
};


