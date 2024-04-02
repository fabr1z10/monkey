#pragma once

#include "nodeaction.h"
#include <glm/glm.hpp>
#include "../components/controllers/sierrafollow.h"

class Turn : public NodeAction {
public:
    Turn(int id, const std::string& direction);
    void start() override;
    int process(double);
private:
    std::string _direction;
};

