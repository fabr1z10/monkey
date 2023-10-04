#pragma once

#include "lines.h"


// parametric curve
// representes a 2D curve z -> (x(z), y(z))
// with given domain and step

class ParametricCurve : public LineModel {
public:
	ParametricCurve() : LineModel() {}
	std::shared_ptr<Renderer> getRenderer(const pybind11::kwargs&) override;
	explicit ParametricCurve(const pybind11::kwargs&);
	int getPoints() const;
	float getStep() const;
	float getInitialPoint() const;

	pybind11::function getFunction();
private:
	float _z0;
	float _z1;
	int _points;
	float _step;
	pybind11::function _func;
};

inline int ParametricCurve::getPoints() const {
	return _points;
}

inline float ParametricCurve::getStep() const {
	return _step;
}

inline float ParametricCurve::getInitialPoint() const {
	return _z0;
}

inline pybind11::function ParametricCurve::getFunction() {
	return _func;
}

class ParametricCurveRenderer : public BatchRenderer<LineBatch> {
public:
	explicit ParametricCurveRenderer(const pybind11::kwargs&);
	void setModel(std::shared_ptr<Model>, const pybind11::kwargs&) override;
	std::type_index getType() override;
	//void start() override;
	void update(double) override;
private:
	std::shared_ptr<ParametricCurve> _lineModel;
};

//
//class TimeVaryingParametricCurve : public LineModel {
//public:
//	TimeVaryingParametricCurve() : LineModel() {}
//	explicit TimeVaryingParametricCurve(const pybind11::kwargs&);
//};
