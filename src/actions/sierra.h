#pragma once

#include "nodeaction.h"

class EnableSierraController : public NodeAction {
public:
	EnableSierraController(int id, bool) ;
	void start() override;
	int process(double) override {return 0;}
private:
	bool _value;
};

class ChangeSierraAnim : public NodeAction {
public:
	ChangeSierraAnim(int id, const std::string& idle, const std::string& walk);
	void start() override;
	int process(double) override {return 0;}
private:
	std::string _idle;
	std::string _walk;
};