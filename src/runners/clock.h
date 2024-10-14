#pragma once

#include "../runner.h"
#include <pybind11/pybind11.h>

class Clock : public Runner {
public:
    Clock();
    void update(double dt);
    // add event to schedule
    int addEvent (bool global, bool relative, long, pybind11::function f);
    void removeEvent(int id);
private:
	void execute(int);
	int _counter;
    static double _globalTime;
    double _roomTime;
    static long _globalSeconds;
    long _roomSeconds;

    static std::unordered_map<long, std::vector<int>> _globalSchedule;
    std::unordered_map<long, std::vector<int>> _roomSchedule;
	static std::unordered_map<int, pybind11::function> _callbacks;
};