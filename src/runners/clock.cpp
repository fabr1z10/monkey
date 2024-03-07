#include "clock.h"

std::unordered_map<long, std::vector<pybind11::function>> Clock::_globalSchedule;
double Clock::_globalTime = 0.0;
long Clock::_globalSeconds = 0;


Clock::Clock() : Runner(), _roomTime(0.0), _roomSeconds(0) {

}

void Clock::update(double dt) {
    _globalTime += dt;
    _roomTime += dt;
    long gs = (long) _globalTime;
    long rs = (long) _roomTime;
    if (gs > _globalSeconds) {
        _globalSeconds = gs;
        auto it = _globalSchedule.find(_globalSeconds);
        if (it != _globalSchedule.end()) {
            for (const auto& func : it->second) func();
            _globalSchedule.erase(_globalSeconds);
        }
    }
    if (rs > _roomSeconds) {
        _roomSeconds = rs;
        auto it = _roomSchedule.find(_roomSeconds);
        if (it != _roomSchedule.end()) {
            for (const auto& func : it->second) func();
            _roomSchedule.erase(_roomSeconds);
        }
    }
    // check events

}

void Clock::addEvent(bool global, bool relative, long t, pybind11::function f) {
    if (global) {
        long time = relative ? _globalSeconds + t : t;
        _globalSchedule[time].push_back(f);
    } else {
        long time = relative ? _roomSeconds + t : t;
        _roomSchedule[time].push_back(f);
    }
}