#pragma once

#include "walk.h"
#include "../components/controller3d.h"
#include "../components/dynamics.h"
#include "../components/sprite_renderer.h"

class Node;

class Walk3D : public Walk {
public:
	explicit Walk3D(const pybind11::kwargs&);
	void init(const pybind11::kwargs& args) override;
	void run(double) override;
	//void keyCallback(GLFWwindow*, int key, int scancode, int action, int mods) override;


};


