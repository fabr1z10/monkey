#pragma once

#include "sierra2d.h"
#include "../../runners/walkarea.h"

class NPCSierraFollow : public Sierra2DController {
public:
    NPCSierraFollow(int target, float speed, float recomputePeriod, const pybind11::kwargs& args);
    void start() override;
    void update(double) override;

private:
    void animate();
    void recomputePath();
    struct WalkSegment {
        glm::vec2 direction;
        float length;
        glm::vec2 arrivalPoint;
    };
    float _speed;
    int _targetId;
    int _currentIndex;
    double _distanceCovered;
    double _time;
    double _recomputeEvery;
    Node * _target;
    WalkArea * _walkArea;
    Renderer* m_animatedRenderer;
    std::string _walkAnim;
    std::string _idleAnim;
    std::vector<WalkSegment> _toGoPoints;
    glm::vec2 _delta;
    bool _moving;
};