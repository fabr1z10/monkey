#include "mousemanager.h"
#include "../engine.h"
#include "../shape.h"
#include "../pyhelper.h"
#include "../models/modelmake.h"
#include <iostream>

MouseArea::MouseArea(std::shared_ptr<Shape> shape, int priority, int camera, const pybind11::kwargs &args)  :
	Component(), _shape(shape), _priority(priority), _camera(camera), _debugNode(nullptr) {
	_onEnter = py_get_dict<pybind11::function>(args, "on_enter", pybind11::function());
	_onLeave = py_get_dict<pybind11::function>(args, "on_leave", pybind11::function());
	_onStay = py_get_dict<pybind11::function>(args, "on_stay", pybind11::function());
	_onClick = py_get_dict<pybind11::function>(args, "on_click", pybind11::function());
	_batchId = py_get_dict<std::string>(args, "batch", "");
}

void MouseArea::setShape(std::shared_ptr<Shape> shape) {
	_shape = shape;
	start();
}

std::type_index MouseArea::getType() {
	return std::type_index(typeid(MouseArea));
}

MouseArea::~MouseArea() {
	Engine::instance().getRoom()->getRunner<MouseManager>()->rmArea(this);
}

void MouseArea::start() {
	Engine::instance().getRoom()->getRunner<MouseManager>()->addArea(this);

	if (!_batchId.empty()) {
		if (_debugNode != nullptr) {
			_debugNode->remove();
		}
		auto &modelMaker = ModelMaker::instance();
		auto model = modelMaker.make(_batchId, _shape, glm::vec4(1.f), FillType::OUTLINE);
		auto node = std::make_shared<Node>();
		node->setModel(model);//pybind11::dict("batch"_a = _batchId));
		node->setPosition(0.f, 0.f, 5.f);
		m_node->add(node);
		_debugNode = node.get();
	}


}

void MouseArea::enter() {
	if (_onEnter) _onEnter(getNode());
}

void MouseArea::leave() {
	if (_onLeave) _onLeave(getNode());
}

void MouseArea::stay() {
	if (_onStay) _onStay(getNode());
}

void MouseArea::click() {
	if (_onClick) _onClick(getNode());
}


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
    //std::cout << x << ", " << y << ", " << _selectedViewport << "\n";
    if (_selectedViewport != -1) {
        _worldCoordinates = _room->getCamera(_selectedViewport)->getWorldCooridnates(vp.x, vp.y);
        auto it = _mouseAreas.find(_selectedViewport);
        MouseArea* _currentArea = nullptr;
        if (it != _mouseAreas.end()) {
        	for (const auto& area : it->second) {
        		// transform point in local coordinates
        		auto localPos = _worldCoordinates - glm::vec2(area->getNode()->getWorldPosition());
        		if (area->getShape()->isInside(glm::vec3(localPos.x, localPos.y, 0.f))) {
        			// found local area!!!
        			if (_currentArea == nullptr || _currentArea->getPriority() > area->getPriority()) {
        				_currentArea = area;
        			}
        		}
        	}
        }
        if (_currentArea != nullptr) {
        	if (_previousArea == nullptr) {
        		_currentArea->enter();
        	} else {
        		if (_previousArea == _currentArea) {
        			_currentArea->stay();
        		} else {
        			_previousArea->leave();
        			_currentArea->enter();
        		}
        	}
        } else {
        	if (_previousArea != nullptr) {
        		_previousArea->leave();
        	}
        }
        _previousArea = _currentArea;



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
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		if (_previousArea != nullptr) {
			_previousArea->click();
		} else {
			// fire
			if (_selectedViewport != -1) {
				auto it = _defaultFunc.find(_selectedViewport);
				if (it != _defaultFunc.end()) {
					it->second(_worldCoordinates.x, _worldCoordinates.y);
				}
			}
		}
	}

}

void MouseManager::start() {
    _room = Engine::instance().getRoom().get();
    _previousArea = nullptr;
}

void MouseManager::update(double) {

}

void MouseManager::addArea(MouseArea * area) {
	_mouseAreas[area->getCamera()].insert(area);
}

void MouseManager::rmArea(MouseArea * area) {
	if (_previousArea == area) {
		_previousArea = nullptr;
	}
	_mouseAreas[area->getCamera()].erase(area);
}