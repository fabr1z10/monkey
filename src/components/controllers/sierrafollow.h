#pragma once

#include "sierra2d.h"
#include "../../runners/walkarea.h"


class WalkableCharacter : public Sierra2DController {
public:
    WalkableCharacter(float speed, const pybind11::kwargs& args);
    void sendMessage(const pybind11::kwargs& args) ;
    void start() override;
    void update(double) override;
    void goTo(glm::vec2);

    using Base = WalkableCharacter;
    bool moving() const;
    void setCallback(pybind11::function);

protected:
    void animate();
    struct WalkSegment {
        glm::vec2 direction;
        float length;
        glm::vec2 arrivalPoint;
    };
    std::vector<WalkSegment> _toGoPoints;
    int _currentIndex;
    bool _moving;
    float _speed;
    double _distanceCovered;
    Renderer* m_animatedRenderer;
    WalkArea * _walkArea;
	int _walkAreaId;
    std::string _idleAnim;
    std::string _walkAnim;
    bool _useAnimDirection;
    glm::vec2 _delta;
    bool _flipHorizontal;
    pybind11::function _customCallback;
};

inline bool WalkableCharacter::moving() const {
    return !_toGoPoints.empty();
}

inline void  WalkableCharacter::setCallback(pybind11::function f) {
    _customCallback = f;
}

class NPCSierraFollow : public WalkableCharacter {
public:
    NPCSierraFollow(pybind11::function f, float speed, float recomputePeriod, const pybind11::kwargs& args);
    void start() override;
    void update(double) override;
    void sendMessage(const pybind11::kwargs& args) override;
    std::type_index getType() {
        return std::type_index(typeid(WalkableCharacter));
    }
private:
    pybind11::function _func;
    void recomputePath();


    int _targetId;

    double _time;
    double _recomputeEvery;
    Node * _target;
    std::string _walkAnim;
    std::string _idleAnim;


};