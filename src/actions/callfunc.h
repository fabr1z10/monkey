#pragma once
#include "../runners/scheduler.h"


class CallFunc : public Action {
public:
	explicit CallFunc(pybind11::function f);
	int run(double) override;
	void start() override {}
private:
	pybind11::function m_func;
};