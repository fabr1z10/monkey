#pragma once

#include "../runner.h"
#include <pybind11/pybind11.h>

class Clock : public Runner {
public:
    Clock();
    void update(double dt);
    // add event to schedule
    void addEvent (bool global, bool relative, long, pybind11::function f);
private:
    static double _globalTime;
    double _roomTime;
    static long _globalSeconds;
    long _roomSeconds;

    static std::unordered_map<long, std::vector<pybind11::function>> _globalSchedule;
    std::unordered_map<long, std::vector<pybind11::function>> _roomSchedule;

};