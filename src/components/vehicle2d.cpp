#include "vehicle2d.h"
#include "../engine.h"
#include "../models/modelmake.h"
#include "../shapes/convexpoly.h"
#include "../shapes/aabb.h"
#include "../util.h"
#include <iostream>

extern GLFWwindow * window;

VehicleController2D::VehicleController2D(float wheelBase, float lengthFront, float lengthBack,
    float width, glm::vec2 wheelSize, float speed, float acceleration, float steerSpeed, float maxSteerAngle, const pybind11::kwargs &args) : Component(args), _wheelBase(wheelBase),
    _width(width), _lengthFront(lengthFront), _lengthBack(lengthBack), _debugShape(nullptr), _hw(_width*0.5f), _steeringAngle(0.f),
    _speed(0.f), _maxSpeed(speed), _steerSpeed(steerSpeed), _wheelRadius(wheelSize.x), _wheelThickness(wheelSize.y),
    _maxSteeringAngle(maxSteerAngle), _acceleration(acceleration) {
    _shape = std::make_shared<shapes::Rect>(
            _wheelBase + _lengthFront + _lengthBack,
            width,
            glm::vec2(_wheelBase + _lengthBack, _hw));
}

void VehicleController2D::start() {
    auto& engine = Engine::instance();
    auto room = engine.getRoom();
    _collisionEngine = room->getRunner<ICollisionEngine>();
    if (engine.drawColliderOutline()) {
        if (_debugShape != nullptr) {
            _debugShape->remove();
        }
        auto batchId = Engine::instance().getColliderOutlineBatch();
        auto& modelMaker = ModelMaker::instance();
        //auto shape = std::make_shared<shapes::AABB>(-_wheelBase - _lengthBack, _lengthFront, -_hw, _hw);
        auto model = modelMaker.make(batchId, _shape, colors::WHITE, FillType::OUTLINE);
        auto node = std::make_shared<Node>();
        node->setModel(model);//pybind11::dict("batch"_a = _batchId));
        m_node->add(node);

        auto wheelModel = modelMaker.make(batchId, std::make_shared<shapes::AABB>(-_wheelRadius, _wheelRadius, -_wheelThickness, _wheelThickness),
            colors::WHITE, FillType::OUTLINE);

        auto addWheel = [node, wheelModel] (float x, float y) {
            auto wheel = std::make_shared<Node>();
            wheel->setPosition(x, y);
            wheel->setModel(wheelModel);
            node->add(wheel);
            return wheel.get();
        };

        // left front wheel
        _lfWheel = addWheel (0.f, _hw);
        _rfWheel = addWheel (0.f, -_hw);
        addWheel(-_wheelBase, _hw);
        addWheel(-_wheelBase, -_hw);




        _debugShape = node.get();
    }
}

void VehicleController2D::update(double dt) {
    // we use the UP key to move vehicle forward (x axis)
    // forward
    bool fwd = glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS;
    // reverse
    bool rev = glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS;

    // steer
    bool left = glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS;
    bool right = glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS;



    if (fwd) {
        _speed += _acceleration * dt;
        if (_speed >= _maxSpeed) _speed = _maxSpeed;
    }
    if (rev) {
        _speed -= _acceleration * dt;
        if (_speed <= -_maxSpeed) _speed = -_maxSpeed;
    }
    if (!fwd && !rev) {
        auto sp = fabs(_speed);
        if (sp < 0.1f) {
            _speed = 0.f;
        } else {
            _speed += -_speed * 0.05f;

        }


    }
    if (left) {
        _steeringAngle += _steerSpeed * dt;
        if (_steeringAngle >= _maxSteeringAngle) {
            _steeringAngle = _maxSteeringAngle;
        }
        //std::cout << _steeringAngle << "\n";
        _lfWheel->setAngle(_steeringAngle);
        _rfWheel->setAngle(_steeringAngle);
    } else if (right) {
        _steeringAngle -= _steerSpeed * dt;
        if (_steeringAngle <= -_maxSteeringAngle) {
            _steeringAngle = -_maxSteeringAngle;
        }
        _lfWheel->setAngle(_steeringAngle);
        _rfWheel->setAngle(_steeringAngle);

    }


    // move/rotate object and check collision only if current speed is non-zero!
    if (_speed != 0.f) {
        glm::vec3 u(cos(_steeringAngle), sin(_steeringAngle), 0.f);
        u *= dt * _speed;
        m_node->moveLocal(u);
        float b = asin(u.y / _wheelBase);
        m_node->rotate(b, glm::vec3(0.f, 0.f, 1.f));

        auto report = _collisionEngine->shapeCast(_shape.get(), m_node->getWorldMatrix(), 0);
        if (!report.empty()) {
            for (const auto& r : report) {
                // separate objects
                std::cout << r.report.direction.x << ", " << r.report.direction.y << ", " << r.report.distance << "\n";
                auto dx = r.report.direction * (r.report.distance * 1.05f);
                m_node->movea(dx);

            }
        }
    }
}

