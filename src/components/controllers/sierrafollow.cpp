#include "sierrafollow.h"
#include "../../engine.h"
#include "../../pyhelper.h"

NPCSierraFollow::NPCSierraFollow(int target, float speed, float recomputePeriod, const pybind11::kwargs& args) :
    Sierra2DController(args), _target(nullptr), _targetId(target), _recomputeEvery(recomputePeriod), _speed(speed) {

    _walkAnim = py_get_dict<std::string>(args, "walk_anim", "walk");
    _idleAnim = py_get_dict<std::string>(args, "idle_anim", "idle");
}

void NPCSierraFollow::start() {
    _target = Engine::instance().getNode(_targetId);
    _walkArea = Engine::instance().getRoom()->getRunner<WalkArea>();
    _time =0.0;
    m_animatedRenderer = m_node->getComponent<Renderer>();

    recomputePath();
}


void NPCSierraFollow::recomputePath() {
    auto mePos = m_node->getWorldPosition();
    auto targetPos =  _target->getWorldPosition();
    if (glm::length(mePos-targetPos) < 1.f) {
        return;
    }
    auto path = _walkArea->findPath(mePos, targetPos);
    _toGoPoints.clear();
    if (!path.empty()) {
        glm::vec2 initialPoint = path.back();
        for (int i = path.size()-2; i >= 0; i--) {
            WalkSegment seg;
            glm::vec2 delta = path[i] - initialPoint;
            seg.direction = glm::normalize(delta);
            seg.length = glm::length(delta);
            seg.arrivalPoint = path[i];
            _toGoPoints.push_back(seg);
        }
    }
    _currentIndex = 0;
    _distanceCovered=0;
}


void NPCSierraFollow::animate() {
    std::string anim = _moving ? "walk" : "idle";
    std::string dir {"_e"};



    if (fabs(_delta.x) < fabs(_delta.y)) {
        if (_delta.y > 0) {
            dir = "_n";
        } else {
            dir = "_s";
        }
    }
    m_animatedRenderer->setAnimation(anim + dir);
}

void NPCSierraFollow::update(double dt) {

    _time += dt;
    if (_time > _recomputeEvery) {
        _time = 0.0;
        recomputePath();
    }

    //_delta = glm::vec2(0, 0);
    _moving = false;
    if (!_toGoPoints.empty() && _currentIndex < _toGoPoints.size()) {
        _moving= true;
        auto& step = _toGoPoints[_currentIndex];
        auto length = _speed * dt;
        _delta = step.direction * static_cast<float>(length);
        _distanceCovered += length;
        m_node->setFlipX(_delta.x < 0);
        if (_distanceCovered >= step.length) {
            m_node->setPosition(step.arrivalPoint.x, step.arrivalPoint.y, 0.f);
            _currentIndex ++;
            _distanceCovered =0.0;
        } else {
            m_node->move(glm::vec3(fabs(_delta.x), _delta.y, 0.0f));
        }

    }

    animate();




    Sierra2DController::update(dt);
}