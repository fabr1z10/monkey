#pragma once

#include "../mouselistener.h"
#include "../runner.h"
#include "../component.h"
#include <pybind11/pybind11.h>
#include <glm/glm.hpp>


class Room;
class Shape;

class MouseArea : public Component {
public:
	MouseArea(std::shared_ptr<Shape> shape, int priority, int camera, const pybind11::kwargs& args);
	~MouseArea() override;
	void start() override;

	[[nodiscard]] int getPriority() const;
	[[nodiscard]] int getCamera() const;
	[[nodiscard]] const std::shared_ptr<Shape> getShape() const;
	void enter();
	void leave();
	void stay();
	void click();
private:
	pybind11::function _onEnter;
	pybind11::function _onLeave;
	pybind11::function _onStay;
	pybind11::function _onClick;
	std::shared_ptr<Shape> _shape;
	int _priority;
	int _camera;
	Node* _debugNode;
	std::string _batchId;

};

inline int MouseArea::getPriority() const {
	return _priority;
}

inline int MouseArea::getCamera() const {
	return _camera;
}

inline const std::shared_ptr<Shape> MouseArea:: getShape() const {
	return _shape;
}


class MouseManager : public Runner, public MouseListener{
public:
    MouseManager() : MouseListener(), Runner(), _room(nullptr) {}
    void update(double) override;
    void start() override;
    void cursorPosCallback(GLFWwindow*, double, double) override;
    void mouseButtonCallback(GLFWwindow*, int, int, int) override;
    void setDefaultCallback(int, pybind11::function);
    void addCamera(int);
    void addArea(MouseArea*);
	void rmArea(MouseArea*);
private:
    std::unordered_map<int, pybind11::function> _defaultFunc;
    int _selectedViewport;
    glm::vec2 _worldCoordinates;
    Room* _room;
    std::vector<int> _cameras; // orderer by priority
    std::unordered_map<int, std::unordered_set<MouseArea*>> _mouseAreas;
    MouseArea* _previousArea;
};