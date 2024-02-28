#include "sierranpc.h"
#include "../../engine.h"
#include "../../pyhelper.h"
#include "../../math/random.h"
#include "../../util.h"
#include <glm/gtx/vector_angle.hpp>

NPCSierraController::NPCSierraController(int target, float fovAngle, float fovRange, float speed,
                                         const pybind11::kwargs &args) : Sierra2DController(args), _targetId(target),
_target(nullptr), _fovAngle(glm::radians(fovAngle)), _fovRange(fovRange), _speed(speed) {
    _lookDirection = glm::normalize(py_get_dict<glm::vec2>(args, "direction", glm::vec2(1.f, 0.f)));
}

void NPCSierraController::start() {
    _target = Engine::instance().getNode(_targetId);
    auto& engine = Engine::instance();
    auto room = engine.getRoom();
    m_collisionEngine = room->getRunner<ICollisionEngine>();
    m_animatedRenderer = m_node->getComponent<Renderer>();

    check_los();
}

void NPCSierraController::check_los(glm::vec3 normal) {
    auto pos = m_node->getWorldPosition();
    auto targetPos = _target->getWorldPosition();
    glm::vec2 directionToTarget = glm::normalize(glm::vec2(targetPos - pos));
    // check if target is in field of view
    // if bouncing n wall, turn to random direction
    std::cout << "current look direction: " << _lookDirection.x << ", " << _lookDirection.y << "; normal = " << normal.x << ", " << normal.y << "\n";
    if (normal != glm::vec3(0.f)) {
        auto angleNormal = glm::orientedAngle(glm::vec2(1.f, 0.f), glm::vec2(normal.x, normal.y));
        float a0 = angleNormal - pi * 0.25f;
        float a1 = angleNormal + pi * 0.25f;
        float angle = Random::instance().getUniformReal(a0, a1);
        _lookDirection = glm::vec2(cos(angle), sin(angle));
        //_lookAngle = angle;
        std::cout << "changed direction, angle normal = " << glm::degrees(angleNormal) << ", new angle " << glm::degrees(angle) << "\n";
    }
    // check if target is in sight
    float angleToTarget = glm::orientedAngle(_lookDirection, directionToTarget);
    bool inSight = false;
    if (fabs(angleToTarget) < _fovAngle * 0.5) {
        float dist = glm::distance(pos, targetPos);
        if (dist <= _fovRange) {
            // target is in FOV
            // check obstacles
            auto raycastResult = m_collisionEngine->rayCast(pos, targetPos, 2, m_node);
            if (!raycastResult.collide) {
                _lookDirection = glm::normalize(targetPos - pos);
                //_lookAngle = angleToTarget;
                std::cout << "target in sight, direction: " << _lookDirection.x << ", " << _lookDirection.y << "\n";

            }
            // if no collision, walk to target pos!!!

        }
    }


    if (fabs(_lookDirection.x) > fabs(_lookDirection.y)) {
        m_animatedRenderer->setAnimation("walk_e");
        m_node->setFlipX(_lookDirection.x < 0);
    } else {
        if (_lookDirection.y > 0) {
            m_animatedRenderer->setAnimation("walk_n");
        } else {
            m_animatedRenderer->setAnimation("walk_s");
        }
        m_node->setFlipX(false);

    }
    //m_animatedRenderer->setTransform(I);

}

void NPCSierraController::update(double dt) {
    auto dtf = static_cast<float>(dt);
    glm::vec2 delta = _lookDirection * dtf * _speed;

    auto P0 = m_node->getWorldPosition();
    auto P1 = P0 + glm::vec3(delta, 0.f);
    auto rr = m_collisionEngine->rayCast(P0, P1, 2, m_node);
    if (rr.collide) {
        delta = _lookDirection * (rr.length - 0.015f);
        check_los(rr.normal);
    }
    if (m_node->getFlipX()) {
        delta.x = -delta.x;
    }
    m_node->move(glm::vec3(delta.x, delta.y, 0.f));
    Sierra2DController::update(dt);

}