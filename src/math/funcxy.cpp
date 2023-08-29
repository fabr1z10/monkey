#include "funcxy.h"
#include "../error.h"

PiecewiseLinearYFunc::PiecewiseLinearYFunc(const std::vector<float> &in) {

	M_Assert(in.size() % 2 == 0, "input size must be a multiple of two");

	for (size_t i = 0; i<in.size(); i+=2) {
		_y.push_back(in[i]);
		_value.push_back(in[i+1]);
	}

}

float PiecewiseLinearYFunc::evaluate(float x, float y) const {
	if (y <= _y.front()) return _value.front();
	if (y >= _y.back()) return _value.back();
	size_t i{0};
	while (_y[i] < y) i++;
	return _value[i-1] + (_value[i] - _value[i-1]) * (y - _y[i-1]) / (_y[i] - _y[i-1]);
}