#pragma once

#include <memory>
#include <functional>
#include "../component.h"
#include "../shape.h"
#include <pybind11/pybind11.h>

class HotSpot : public Component {
public:
	HotSpot(std::shared_ptr<Shape>, const pybind11::kwargs& args);
	virtual ~HotSpot();
	//void update(double) override;
	void start() override;
	const Shape* const getShape() const;
	virtual void onEnter ();
	virtual void onLeave ();
	virtual void onClick (glm::vec2 pos, int button, int action);

	void setOnEnter(pybind11::function);
	void setOnLeave(pybind11::function);
	void setOnClick(pybind11::function);

	std::string getBatch() const;
protected:
	std::shared_ptr<Shape> _shape;
	std::string _batchId;

	pybind11::function _onEnter;
	pybind11::function _onLeave;
	pybind11::function _onClick;
};

inline const Shape *const HotSpot::getShape() const {
	return _shape.get();
}

inline void HotSpot::setOnEnter(pybind11::function f) {
	_onEnter = f;
}

inline void HotSpot::setOnLeave(pybind11::function f) {
	_onLeave = f;
}

inline void HotSpot::setOnClick(pybind11::function f) {
	_onClick = f;
}

inline std::string HotSpot::getBatch() const {
	return _batchId;
}