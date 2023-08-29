#pragma once

#include <vector>

class FuncXY {
public:
	virtual float evaluate(float x, float y) const = 0;
};

class PiecewiseLinearYFunc : public FuncXY {
public:
	PiecewiseLinearYFunc(const std::vector<float>& in);
	float evaluate(float x, float y) const override;
private:
	std::vector<float> _y;
	std::vector<float> _value;
};