#pragma once

#include <vector>
#include <memory>
#include <pybind11/pybind11.h>
#include <glm/glm.hpp>

class FuncXY {
public:
	virtual float evaluate(float x, float y) const = 0;
};

class FuncConst : public FuncXY {
public:
	FuncConst(const pybind11::kwargs&);
	float evaluate(float x, float y) const override {
		return _value;
	}
private:
	float _value;
};

class PiecewiseLinearFunc : public FuncXY {
public:
	PiecewiseLinearFunc(const pybind11::kwargs&);
	float evaluate(float x, float y) const override;
private:
	int _var;
	std::vector<float> _u;
	std::vector<float> _value;
};

class GridFunction : public FuncXY {
public:
	GridFunction(const pybind11::kwargs&);
	//void addFunction(std::shared_ptr<FuncXY> func);
	float evaluate(float x, float y) const override;

private:
	std::vector<glm::vec4> _boxes;
	std::vector<std::shared_ptr<FuncXY>> _funcs;
	std::shared_ptr<FuncXY> _defaultFunc;
};