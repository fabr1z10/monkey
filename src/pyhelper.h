#pragma once

#include <pybind11/pybind11.h>
#include "pycast.h"
#include <glm/glm.hpp>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>

namespace py = pybind11;





//
//
//
//
//
//template<typename T>
//T py_get(pybind11::object obj) {
//	return obj.cast<T>();
//}
//
//template<typename T>
//T py_get(pybind11::object obj, T defaultValue) {
//    try {
//        return obj.cast<T>();
//    } catch (...) {
//        return defaultValue;
//    }
//}


template<typename T>
T py_get(const pybind11::handle& obj, const std::string& key) {
	return obj.attr(key.c_str()).cast<T>();
}

template <typename T>
T py_get_dict(const pybind11::handle& obj, const std::string& key) {
    return obj[key.c_str()].cast<T>();
}


template<typename T>
T py_get(const pybind11::handle& obj, const std::string& key, T defaultValue) {
	try {
		auto value = py_get<T>(obj, key);
		return value;
	} catch (...) {
		return defaultValue;
	}
}

template<typename T>
T py_get_dict(const pybind11::handle& obj, const std::string& key, T defaultValue) {
    try {
        auto value = py_get_dict<T>(obj, key);
        return value;
    } catch (...) {
        return defaultValue;
    }
}


//template<>
//inline glm::vec2 py_get(pybind11::object obj) {
//	auto vec = obj.cast<std::vector<float>>();
//	return glm::vec2(vec[0], vec[1]);
//}
//
//template<>
//inline glm::vec3 py_get(pybind11::object obj) {
//	auto vec = obj.cast<std::vector<float>>();
//	return glm::vec3(vec[0], vec[1], vec[2]);
//}
//
//template<>
//inline glm::vec4 py_get(pybind11::object obj) {
//	auto vec = obj.cast<std::vector<float>>();
//	return glm::vec4(vec[0], vec[1], vec[2], vec[3]);
//}

//template<>
//inline glm::ivec2 py_get(pybind11::object obj) {
//	auto vec = obj.cast<std::vector<int>>();
//	return glm::ivec2(vec[0], vec[1]);
//}

