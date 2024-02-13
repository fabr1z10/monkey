#pragma once

#include "../component.h"
#include <vector>
#include <memory>

class Switch : public Component
{
public:
    Switch();
    void start() override;
    void add(std::shared_ptr<Component>);
    void update(double) override;
    void enable(int);
    using Base = Switch;

private:
    int _activeComponent;
    std::vector<std::shared_ptr<Component>> _components;

};