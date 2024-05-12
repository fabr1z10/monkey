#include "sierrafollow.h"
#include "../../engine.h"
#include "../../pyhelper.h"


WalkableCharacter::WalkableCharacter(float speed, const pybind11::kwargs &args) : Sierra2DController(args), _speed(speed) {
    _idleAnim = py_get_dict<std::string>(args, "idle_anim", "idle");
    _walkAnim = py_get_dict<std::string>(args, "walk_anim", "walk");
    _useAnimDirection = py_get_dict<bool>(args, "anim_dir", true);
    _flipHorizontal = py_get_dict<bool>(args, "flip_horizontal", true);
    _direction = py_get_dict<std::string>(args, "direction", "e");
}

void WalkableCharacter::sendMessage(const pybind11::kwargs &args) {
    auto id = py_get_dict<std::string>(args, "id");
    if (id == "goto") {
        goTo(py_get_dict<glm::vec2>(args, "pos"));
    } else if (id == "animate") {
    	_idleAnim = py_get_dict<std::string>(args, "anim");
    }

}

void NPCSierraFollow::sendMessage(const pybind11::kwargs &args) {
    auto id = py_get_dict<std::string>(args, "id");
    if (id == "setFunc") {
        _func = py_get_dict<pybind11::function>(args, "func", pybind11::function());
    }
}


NPCSierraFollow::NPCSierraFollow(pybind11::function f, float speed, float recomputePeriod,
                                 const pybind11::kwargs& args) :
    WalkableCharacter(speed, args), _target(nullptr), _func(f), _recomputeEvery(recomputePeriod) {

    _walkAnim = py_get_dict<std::string>(args, "walk_anim", "walk");
    _idleAnim = py_get_dict<std::string>(args, "idle_anim", "idle");
}

void WalkableCharacter::start() {
    _walkArea = Engine::instance().getRoom()->getRunner<WalkArea>();

    m_animatedRenderer = m_node->getComponent<Renderer>();
	m_node->setFlipX(_direction == "w");
}

void NPCSierraFollow::start() {
    WalkableCharacter::start();
    //_target = Engine::instance().getNode(_targetId);
    _time =0.0;
    recomputePath();

}




void WalkableCharacter::goTo(glm::vec2 targetPos) {
    glm::vec2 mePos(m_node->getWorldPosition());

    if (glm::length(mePos-targetPos) < 1.f) {
        return;
    }
    auto path = _walkArea->findPath(mePos, targetPos);
    _toGoPoints.clear();
    if (!path.empty()) {
        glm::vec2 initialPoint = path.back();
        for (int i = path.size()-2; i >= 0; i--) {
            WalkSegment seg;
            glm::vec2 delta = path[i] - path[i+1];
            seg.direction = glm::normalize(delta);
            seg.length = glm::length(delta);
            seg.arrivalPoint = path[i];
            _toGoPoints.push_back(seg);
        }
    }
    _currentIndex = 0;
    _distanceCovered=0;
}

void NPCSierraFollow::recomputePath() {
    if (_func) {
        auto targetPos = _func().cast<glm::vec2>();
        goTo(targetPos);
    }




}


void WalkableCharacter::animate() {
    std::string anim = _moving ? _walkAnim : _idleAnim;
    if (_useAnimDirection) {
    	if (_moving) {
			if (fabs(_delta.x) < fabs(_delta.y)) {
				if (_delta.y > 0) {
					_direction = "n";
				} else {
					_direction = "s";
				}
			} else {
			    _direction = (_delta.x > 0.f) ? "e" : "w";
			}
		}
    	m_node->setFlipX(_direction == "w");
    	auto dir = (_direction == "w" ? "e" : _direction);
    	//if (_direction == "w") {
		//	m_node->setFlipX(true);
    	//	_direction = "e";
    	//}
		anim += "_" + dir;
    }
    m_animatedRenderer->setAnimation(anim);
}

void WalkableCharacter::update(double dt) {

    //_delta = glm::vec2(0, 0);
    _moving = false;
    if (!_toGoPoints.empty() && _currentIndex < _toGoPoints.size()) {
        _moving= true;
        auto& step = _toGoPoints[_currentIndex];
        auto length = _speed * dt;
        _delta = step.direction * static_cast<float>(length);
        _distanceCovered += length;
        float dx = _delta.x;
        if (_flipHorizontal) {
            m_node->setFlipX(_delta.x < 0);
            dx = fabs(dx);
        }
        if (_distanceCovered >= step.length) {
            //std::cout << " moved to " << step.arrivalPoint.x << ", " << step.arrivalPoint.y << "\n";
            m_node->setPosition(step.arrivalPoint.x, step.arrivalPoint.y, 0.f);
            _currentIndex ++;
            _distanceCovered =0.0;
            if (_currentIndex >= _toGoPoints.size()) {
                _toGoPoints.clear();
            }
        } else {

            m_node->move(glm::vec3(dx, _delta.y, 0.0f));
        }

    }

    animate();

    Sierra2DController::update(dt);
}

void NPCSierraFollow::update(double dt) {

    _time += dt;
    if (_time > _recomputeEvery || _toGoPoints.empty()) {
        _time = 0.0;
        recomputePath();
    }
    WalkableCharacter::update(dt);
    //_delta = glm::vec2(0, 0);
//    _moving = false;
//    if (!_toGoPoints.empty() && _currentIndex < _toGoPoints.size()) {
//        _moving= true;
//        auto& step = _toGoPoints[_currentIndex];
//        auto length = _speed * dt;
//        _delta = step.direction * static_cast<float>(length);
//        _distanceCovered += length;
//        m_node->setFlipX(_delta.x < 0);
//        if (_distanceCovered >= step.length) {
//            m_node->setPosition(step.arrivalPoint.x, step.arrivalPoint.y, 0.f);
//            _currentIndex ++;
//            _distanceCovered =0.0;
//        } else {
//            m_node->move(glm::vec3(fabs(_delta.x), _delta.y, 0.0f));
//        }
//
//    }
//
//    animate();
//
//
//
//
//    Sierra2DController::update(dt);
}