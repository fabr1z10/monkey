#include "walk3d.h"
#include <GLFW/glfw3.h>
#include "../../node.h"
#include "../../engine.h"

extern GLFWwindow* window;

Walk3DController::Walk3DController(float size, float speed, float gravity) : Component(), _size(size), _speed(speed), _gravity(gravity), _lookingLeft(false) {
    _xCorrection = 0.f; //sqrt(2.f) * 0.5f;
    _yVelocity = 0.f;
    _skinWidth =  .015f;

}

void Walk3DController::start() {
    //_dir = 'e';
    _lookingLeft = false;

    auto& engine = Engine::instance();
    auto room = engine.getRoom();
    m_collisionEngine = room->getRunner<ICollisionEngine>();
}

void Walk3DController::update(double dt) {
    float dtf = static_cast<float>(dt);
    auto leftPressed = glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS;
    auto rightPressed = glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS;
    auto upPressed = glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS;
    auto downPressed = glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS;
    auto anyPressed = (leftPressed || rightPressed || upPressed || downPressed);

    if (leftPressed)
        _lookingLeft = true;

    if (rightPressed)
        _lookingLeft = false;

    float dx = (leftPressed || rightPressed ? 1.f : 0.f);
    float dz = upPressed ? -1.f : (downPressed ? 1.f : 0.f);
    if (upPressed) dx -= _xCorrection;
    if (downPressed) dx += _xCorrection;

    glm::vec3 direction(dx, 0, dz);
    if (dx != 0 || dz != 0) {
        direction = glm::normalize(direction);
        direction *= _speed;
        dx = direction.x;
        dz = direction.z;
    }

    _yVelocity -= _gravity * dtf;
    float dy = _yVelocity * dtf;

    m_node->setFlipX(_lookingLeft);
    float dir_x = _lookingLeft ? -1.f : 1.f;
    float dir_z = (dz > 0) ? 1.f : -1.f;
    auto pos = m_node->getWorldPosition();
    if (dx > 0) {
        // check horizontal
        glm::vec3 rayOrigin(pos.x + (_lookingLeft ? -_size : _size), pos.y, pos.z);
        float rayLength = dx * (_lookingLeft ? -1.f : 1.f);
        RayCastHit hit = m_collisionEngine->rayCastX(rayOrigin, rayLength, 2, m_node);
        if (hit.collide) {
            dx = hit.length - _skinWidth;
        }
    }

    if (dz != 0.f) {
        glm::vec3 rayOriginLeft(pos.x + dx * dir_x - _size, pos.y, pos.z - _size);
        glm::vec3 rayOriginRight(pos.x + dx * dir_x + _size, pos.y, pos.z + _size);
        auto hit_left = m_collisionEngine->rayCastZ(rayOriginLeft, dz, 2, m_node);
        if (hit_left.collide) {
            dz = dir_z * (hit_left.length - _skinWidth);
        }
        auto hit_right = m_collisionEngine->rayCastZ(rayOriginRight, dz, 2, m_node);
        if (hit_right.collide) {
            dz = dir_z * (hit_right.length - _skinWidth);
        }
    }

    // vertical collision
    if (dy < 0.f) {
        glm::vec3 rayOriginLeft(pos.x + dx * dir_x - _size, pos.y, pos.z + dir_z * dz);
        glm::vec3 rayOriginRight(pos.x + dx * dir_x + _size, pos.y, pos.z + dir_z * dz);
        auto hit_left = m_collisionEngine->rayCastY(rayOriginLeft, dy, 2, m_node);
        if (hit_left.collide) {
            dy = - (hit_left.length - _skinWidth);
        }
        auto hit_right = m_collisionEngine->rayCastY(rayOriginRight, dy, 2, m_node);
        if (hit_right.collide) {
            dy = - (hit_right.length - _skinWidth);
        }
        if (hit_left.collide || hit_right.collide) {
            _yVelocity = 0.f;
        }
    }

    m_node->move(glm::vec3(dx, dy, dz));


}