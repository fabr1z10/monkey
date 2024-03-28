#pragma once

#include "nodeaction.h"
#include <glm/glm.hpp>
#include "../components/controllers/sierrafollow.h"

class Walk : public NodeAction {
public:
    Walk(int id, glm::vec2 target);
    void start() override;
    int process(double);
private:
    glm::vec2 _target;
    WalkableCharacter* _char;
};

class WalkDynamic : public NodeAction {
public:
    WalkDynamic(int id, pybind11::function f);
    void start() override;
    int process(double);
private:
    pybind11::function  _func;
    WalkableCharacter* _char;
};