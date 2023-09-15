#pragma once

#include <memory>
#include <functional>
#include "../component.h"
#include "../shape.h"
#include <pybind11/pybind11.h>

class HotSpot : public Component {
public:
	HotSpot(std::shared_ptr<Shape>, const pybind11::kwargs& args);
	~HotSpot() override;
	//void update(double) override;
	void start() override;
	const Shape* const getShape() const;
	virtual void onEnter () = 0;
	virtual void onLeave () = 0;
	virtual void onClick (glm::vec2 pos, int button, int action) = 0;

	int getPriority() const;
	std::string getBatch() const;
protected:
	std::shared_ptr<Shape> _shape;
	std::string _batchId;

	int _priority;
};

class PyHotSpot : public HotSpot {
public:
	PyHotSpot(std::shared_ptr<Shape>, const pybind11::kwargs& args);
	void setOnEnter(pybind11::function);
	void setOnLeave(pybind11::function);
	void setOnClick(pybind11::function);
	void onEnter () override;
	void onLeave () override;
	void onClick (glm::vec2 pos, int button, int action) override;
private:
	pybind11::function _onEnter;
	pybind11::function _onLeave;
	pybind11::function _onClick;

};



inline int HotSpot::getPriority() const {
	return _priority;
}
inline const Shape *const HotSpot::getShape() const {
	return _shape.get();
}

inline void PyHotSpot::setOnEnter(pybind11::function f) {
	_onEnter = f;
}

inline void PyHotSpot::setOnLeave(pybind11::function f) {
	_onLeave = f;
}

inline void PyHotSpot::setOnClick(pybind11::function f) {
	_onClick = f;
}

inline std::string HotSpot::getBatch() const {
	return _batchId;
}