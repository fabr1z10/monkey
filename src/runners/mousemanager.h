#pragma once

#include "../mouselistener.h"
#include "../runner.h"
#include <pybind11/pybind11.h>
#include <glm/glm.hpp>


class Room;

class MouseManager : public Runner, public MouseListener{
public:
    MouseManager() : MouseListener(), Runner(), _room(nullptr) {}
    void update(double) override;
    void start() override;
    void cursorPosCallback(GLFWwindow*, double, double) override;
    void mouseButtonCallback(GLFWwindow*, int, int, int) override;
    void setDefaultCallback(int, pybind11::function);
private:
    std::unordered_map<int, pybind11::function> _defaultFunc;
    int _selectedViewport;
    glm::vec2 _worldCoordinates;
    Room* _room;
};