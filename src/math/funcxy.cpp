#include "funcxy.h"
#include "../error.h"
#include "../pyhelper.h"


FuncConst::FuncConst(const pybind11::kwargs & args) {
	_value = py_get_dict<float>(args, "value");
}

PiecewiseLinearFunc::PiecewiseLinearFunc(const pybind11::kwargs& args) {
	_var = py_get_dict<int>(args, "var");
	auto in = py_get_dict<std::vector<float>>(args, "data");
	M_Assert(in.size() % 2 == 0, "input size must be a multiple of two");

	for (size_t i = 0; i<in.size(); i+=2) {
		_u.push_back(in[i]);
		_value.push_back(in[i+1]);
	}

}

float PiecewiseLinearFunc::evaluate(float x, float y) const {
	float z = (_var == 0 ? x : y);
	if (z <= _u.front()) return _value.front();
	if (z >= _u.back()) return _value.back();
	size_t i{0};
	while (_u[i] < z) i++;
	return _value[i-1] + (_value[i] - _value[i-1]) * (z - _u[i-1]) / (_u[i] - _u[i-1]);
}

GridFunction::GridFunction(const pybind11::kwargs& args) {
	for (const auto& box : args["boxes"]) {
		auto bd = box.cast<pybind11::dict>();
		auto bounds = py_get_dict<glm::vec4>(bd, "bounds");
		auto func = py_get_dict<std::shared_ptr<FuncXY>>(bd, "func");
		_boxes.push_back(bounds);
		_funcs.push_back(func);
	}

	_defaultFunc = py_get_dict<std::shared_ptr<FuncXY>>(args, "default", std::shared_ptr<FuncXY>());

}
//
//void GridFunction::addFunction(std::shared_ptr<FuncXY> func) {
//	_funcs.push_back(func);
//}

float GridFunction::evaluate(float x, float y) const {

	for (size_t i = 0; i < _boxes.size(); ++i) {
		if (x >= _boxes[i][0] && x <= _boxes[i][2] && y >= _boxes[i][1] && y <= _boxes[i][3]) {
			return _funcs[i]->evaluate(x, y);
		}
	}
	if (_defaultFunc == nullptr)
		return 0.f;
	return _defaultFunc->evaluate(x, y);

}