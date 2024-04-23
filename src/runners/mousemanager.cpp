#include "mousemanager.h"
#include "../engine.h"
#include <iostream>

void MouseManager::cursorPosCallback(GLFWwindow *, double x, double y) {
    // first, find which viewport the mouse is in (if any)
    _selectedViewport = -1;
    if (_room == nullptr) return;
    glm::vec2 vp = Camera::getViewportCoordinates(x, y);
    for (int i : _cameras) {
        if (_room->getCamera(i)->isInViewport(vp.x, vp.y)) {
            _selectedViewport = i;
            break;
        }

    }
    std::cout << x << ", " << y << ", " << _selectedViewport << "\n";
    if (_selectedViewport != -1) {
        _worldCoordinates = _room->getCamera(_selectedViewport)->getWorldCooridnates(vp.x, vp.y);
//        auto it = _defaultFunc.find(_selectedViewport);
//        if (it != _defaultFunc.end()) {
//            it->second(worldCoordinates.x, worldCoordinates.y);
//        }
    }
}

void MouseManager::addCamera(int i) {
	_cameras.push_back(i);
}

void MouseManager::setDefaultCallback(int i, pybind11::function f) {
    _defaultFunc[i] = f;
}

void MouseManager::mouseButtonCallback(GLFWwindow *, int button, int action, int mods) {
    // look if hotspot is selected, otherwise...
    // fire
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        if (_selectedViewport != -1) {
            auto it = _defaultFunc.find(_selectedViewport);
            if (it != _defaultFunc.end()) {
                it->second(_worldCoordinates.x, _worldCoordinates.y);
            }
        }
    }

}

void MouseManager::start() {
    _room = Engine::instance().getRoom().get();
}

void MouseManager::update(double) {

}