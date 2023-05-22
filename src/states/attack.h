#pragma once

#include "../components/statemachine.h"
#include "../components/controller2d.h"
#include "../components/dynamics.h"
#include "../components/sprite_renderer.h"

class Node;

class Attack : public State {
public:
    Attack() : State() {}
    void start() override;

    void setParent(StateMachine*, const pybind11::kwargs& args) override;
    void control();
    void init(const pybind11::kwargs& args) override;
    void run(double) override;
    void keyCallback(int key) override;

protected:
    float m_maxSpeedGround;
    float m_maxSpeedAir;
    float m_gravity;
    float m_jumpHeight;
    float m_timeToJumpApex;
    float m_jumpVelocity;           // calculated
    float m_acceleration;
    float m_accelerationAir;
    float m_accelerationTime;

    Controller2D* m_controller;
    Dynamics* m_dynamics;
    Node* m_node;
    Renderer* m_animatedRenderer;
    std::string m_anim;
    bool m_left;
    bool m_right;
    bool m_up;
};


