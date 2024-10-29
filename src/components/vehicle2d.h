#pragma once

#include "../runners/collision_engine.h"
#include "../shape.h"


class VehicleController2D : public Component {
public:
    VehicleController2D(float wheelBase, float lengthFront, float lengthBack,
                        float width, glm::vec2 wheelSize, float speed, float acceleration, float steerSpeed,
                        float maxSteeringAngle, const pybind11::kwargs& args);
    void start() override;

    void update(double) override;

private:
    ICollisionEngine* _collisionEngine;
    float _width;
    float _lengthFront, _lengthBack;
    float _wheelBase;
    float _hw;
    float _speed;
    float _maxSpeed;
    float _acceleration;
    Node* _debugShape;
    float _steeringAngle;
    float _steerSpeed;
    float _wheelRadius;
    float _wheelThickness;
    Node* _lfWheel;
    Node* _rfWheel;
    float _maxSteeringAngle;
    std::shared_ptr<shapes::Shape> _shape;
};