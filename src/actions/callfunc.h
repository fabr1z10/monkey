#pragma once
#include "../runners/scheduler.h"


class CallFunc : public Action {
public:
	explicit CallFunc(pybind11::function f);
	void start() override;
	int process(double) override;
private:
	pybind11::function m_func;
};