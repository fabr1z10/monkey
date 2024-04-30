#include "callfunc.h"

CallFunc::CallFunc(pybind11::function f) {
	m_func = std::move(f);
}


void CallFunc::start() {
	m_func();

}

int CallFunc::process(double) {
	return 0;
}