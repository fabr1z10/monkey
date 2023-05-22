#include "attack.h"
#include <GLFW/glfw3.h>
#include "../util.h"
#include "../node.h"

extern GLFWwindow* window;

void Attack::setParent(StateMachine* sm, const pybind11::kwargs &kwargs) {
    State::setParent(sm, kwargs);
    // lines below same as walk2d...
    m_gravity = py_get_dict<float>(kwargs, "gravity", m_sm->getProperty<float>("gravity"));
    m_jumpHeight = py_get_dict<float>(kwargs, "jump_height", m_sm->getProperty<float>("jump_height"));
    m_timeToJumpApex = py_get_dict<float>(kwargs, "time_to_jump_apex", m_sm->getProperty<float>("time_to_jump_apex"));
    m_jumpVelocity = (m_jumpHeight + 0.5f * m_gravity * m_timeToJumpApex * m_timeToJumpApex) / m_timeToJumpApex;
    m_maxSpeedGround = py_get_dict<float>(kwargs, "speed", m_sm->getProperty<float>("speed"));
    m_maxSpeedAir = py_get_dict<float>(kwargs, "speed_air", m_maxSpeedGround);
    m_accelerationTime = py_get_dict<float>(kwargs, "acc_time", m_sm->getProperty<float>("acc_time"));
    if (m_accelerationTime == 0.f) {
        m_acceleration = 0.f;
        m_accelerationAir = 0.f;
    }
    else {
        m_acceleration = m_maxSpeedGround / m_accelerationTime;
        m_accelerationAir = m_maxSpeedAir / m_accelerationTime;
    }

    m_anim = py_get_dict<std::string>(kwargs, "anim");

}

void Attack::start() {
    auto node = m_sm->getNode();

    m_controller = dynamic_cast<Controller2D*>(node->getComponent<Controller>());
    assert(m_controller != nullptr);

    m_dynamics = node->getComponent<Dynamics>();
    assert(m_dynamics != nullptr);

    //m_animatedRenderer = dynamic_cast<SpriteRenderer*>(m_node->getComponent<Renderer>());
    m_animatedRenderer = node->getComponent<Renderer>();
}

void Attack::init(const pybind11::kwargs& args) {
    State::init(args);
    m_animatedRenderer->setAnimation(m_anim);
}

void Attack::run(double dt) {
    auto dtf = static_cast<float>(dt);

    control();

    float maxSpeed {0.f};
    float acceleration{0.f};
    if (m_controller->grounded()) {
        maxSpeed = m_maxSpeedGround;
        acceleration = m_acceleration;
        if (m_up) {
            m_dynamics->m_velocity.y = m_jumpVelocity;
        } else {
            m_dynamics->m_velocity.y = 0.0f;
        }
    } else {
        // bump head
        maxSpeed = m_maxSpeedAir;
        acceleration = m_accelerationAir;
        if (m_controller->ceiling()) {
            m_dynamics->m_velocity.y = 0;
        }

    }


    glm::vec3 a(0.0f);
    a.y = -m_gravity;

    if (m_left || m_right) {
        a.x = acceleration; // (m_left ? -1.f : 1.f) * m_acceleration;
    } else {
        // apply deceleration only if velocity above threshold
        if (fabs(m_dynamics->m_velocity.x) > 0.1f) {
            a.x = - signf(m_dynamics->m_velocity.x) * acceleration;
        } else {
            a.x = 0.0f;
            m_dynamics->m_velocity.x = 0.0f;
        }
    }

    m_dynamics->m_velocity += a * dtf;

    // limit horizontal vel to max speed
    if (m_left || m_right) {
        if (fabs(m_dynamics->m_velocity.x) > maxSpeed) {
            m_dynamics->m_velocity.x = signf(m_dynamics->m_velocity.x) * maxSpeed;
        }
    }


    auto delta = m_dynamics->m_velocity * dtf;

    m_controller->move(delta, false);

    // update animation, if we have a sprite renderer
    if (m_animatedRenderer->isComplete()) {
        m_sm->setState("walk");
    }
//    if (m_animatedRenderer) {
//        if (m_controller->grounded()) {
//            if (fabs(m_dynamics->m_velocity.x) < 0.1f) {
//                m_animatedRenderer->setAnimation(m_idleAnim);
//            } else {
//                m_animatedRenderer->setAnimation(m_walkAnim);
//            }
//        } else {
//            // jump or fall anim?
//            m_animatedRenderer->setAnimation(m_jumpAnim);
//        }
//
//
//    }
}

void Attack::control() {

    m_left = glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS;
    m_right = glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS;
    m_up = glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS;
}

void Attack::keyCallback(int key) {

    switch (key) {
        case GLFW_KEY_LEFT:
            m_sm->getNode()->setFlipX(true);
            break;
        case GLFW_KEY_RIGHT:
            m_sm->getNode()->setFlipX(false);
            break;
    }
    State::keyCallback(key);
}
