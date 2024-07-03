#include "clock.h"

std::unordered_map<long, std::vector<int>> Clock::_globalSchedule;
double Clock::_globalTime = 0.0;
long Clock::_globalSeconds = 0;


Clock::Clock() : Runner(), _roomTime(0.0), _roomSeconds(0), _counter(0) {

}


void Clock::execute(int id) {
	auto it = _callbacks.find(id);
	if (it != _callbacks.end()) {
		it->second();
	}
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
            for (const auto& func : it->second) {
				execute(func);
            }
            _globalSchedule.erase(_globalSeconds);
        }
    }
    if (rs > _roomSeconds) {
        _roomSeconds = rs;
        auto it = _roomSchedule.find(_roomSeconds);
        if (it != _roomSchedule.end()) {
            for (const auto& func : it->second) {
            	execute(func);
            }
            _roomSchedule.erase(_roomSeconds);
        }
    }
    // check events

}

int Clock::addEvent(bool global, bool relative, long t, pybind11::function f) {
	int id = _counter;
	_callbacks[id] = f;
    if (global) {
        long time = relative ? _globalSeconds + t : t;
        _globalSchedule[time].push_back(id);
    } else {
        long time = relative ? _roomSeconds + t : t;
        _roomSchedule[time].push_back(id);
    }
    _counter++;
    return id;
}

void Clock::removeEvent(int id) {
	_callbacks.erase(id);
}