#pragma once

#include "sierra2d.h"
#include "../../runners/walkarea.h"

class NPCSierraFollow : public Sierra2DController {
public:
    NPCSierraFollow(int target, const pybind11::kwargs& args);
    void start() override;
    void update(double) override;
private:

    int _targetId;
    Node * _target;
    WalkArea * _walkArea;
};