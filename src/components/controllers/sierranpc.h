#pragma once

#include "sierra2d.h"

class NPCSierraController : public Sierra2DController {

public:
    NPCSierraController(int target, float fovAngle, float fovRange, float speed, const pybind11::kwargs& args);
    void start() override;
    void update(double) override;

private:
    void check_los(glm::vec3 normal = glm::vec3(0.f));
    ICollisionEngine* m_collisionEngine;

    int _targetId;
    Node * _target;
    float _fovAngle;
    float _speed;
    float _fovRange;
    glm::vec2 _lookDirection;
};