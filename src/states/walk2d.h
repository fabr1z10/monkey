#pragma once

#include "walk.h"


class Node;

class Walk2D : public Walk {
public:
	explicit Walk2D(const pybind11::kwargs&);
	//void start() override;
	void init(const pybind11::kwargs& args) override;
	void run(double) override;
	//void keyCallback(GLFWwindow*, int key, int scancode, int action, int mods) override;

};


