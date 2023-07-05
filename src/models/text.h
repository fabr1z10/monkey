#pragma once


#include "iquad.h"
#include <pybind11/pybind11.h>

class Text : public IQuad {
public:
	Text(const pybind11::kwargs&);
    // text requires: a font, a text, a palette
    // then we might have some subclass ... like text appearing one character at a time
    //void initFromPy(const pybind11::kwargs&) override;
    //void initFromYAML(const YAML::Node &node, const YAML::Node &globals) override { throw; }


private:

};