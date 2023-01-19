#pragma once

#include "../runners/collision.h"
#include "../shape.h"
#include <unordered_map>
#include <functional>
#include <utility>
#include "../hashpair.h"

class Intersector {
public:
    Intersector() {}
    CollisionReport intersect(const Shape*, const Shape*, const glm::mat4&, const glm::mat4&) ;

    template<typename T, typename S>
    void add(std::function<CollisionReport(const Shape*, const Shape*, const glm::mat4&, const glm::mat4&)> f) {
        m_functionMap[std::make_pair(std::type_index(typeid(T)), std::type_index(typeid(S)))] = std::move(f);
    }


protected:
    std::unordered_map<std::pair<std::type_index, std::type_index>, std::function<CollisionReport(const Shape*, const Shape*, const glm::mat4&, const glm::mat4&)>> m_functionMap;
};


