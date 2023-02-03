#include "callfunc.h"

CallFunc::CallFunc(pybind11::function f) {
	m_func = std::move(f);
}



int CallFunc::run(double) {
	m_func();
	return 0;
}