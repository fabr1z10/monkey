#pragma once

#include <typeindex>

class Runner {
public:
    Runner() = default;
    virtual ~Runner() = default;
    virtual void start() {}
    virtual void update(double) = 0;
    virtual std::type_index getType();
};

inline std::type_index Runner::getType() {
    return std::type_index(typeid(*this));
}
