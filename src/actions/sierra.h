#pragma once

#include "nodeaction.h"

class EnableSierraController : public NodeAction {
public:
	EnableSierraController(const pybind11::kwargs& args) ;
	void start() override;
	int process(double) override {return 0;}
private:
	bool _value;
};

class ChangeSierraAnim : public NodeAction {
public:
	ChangeSierraAnim(const pybind11::kwargs& args);
	void start() override;
	int process(double) override {return 0;}
private:
	std::string _idle;
	std::string _walk;
};