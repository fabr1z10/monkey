#pragma once

#include <vector>
#include <sstream>
#include <glm/glm.hpp>

constexpr double pi = 3.14159265358979323846;

const int key_left = 0x1;
const int key_right = 0x2;
const int key_up =  0x4;
const int key_down = 0x8;
const int key_jmp = 0x10;


const float epsilon = 0.0001f;

template <class T>
bool isEqual (T x, T y, T eps = epsilon) {
    return std::abs(x-y) < eps;
}

template <class T>
bool isZero (T x, T eps = epsilon) {
    return isEqual(x, 0.0f, eps);
}


template<typename T>
std::vector<T> strToVec(const std::string& s, char delim=',') {
    std::stringstream s_stream(s); //create string stream from the string
    std::vector<T> out;
    while(s_stream.good()) {
        std::string substr;
        getline(s_stream, substr, delim); //get first string delimited by comma
        std::stringstream ss;
        ss << substr;
        T value;
        ss >> value;
        out.push_back(value);

    }
    return out;

}

std::u32string getString32(const std::string&);

// returns the angle (in radians) between two vectors
float angle(glm::vec3 v1, glm::vec3 v2) ;

// returns 1 if f >= 0, -1 otherwise
inline int sign (float f) {
    return f >= 0.0f ? 1 : -1;
}

// returns 1 if f >= 0, -1 otherwise
inline float signf (float f) {
    return f >= 0.0f ? 1.f : -1.f;
}

float computeOverlap(glm::vec2& p1, glm::vec2& p2);

bool seg2seg(glm::vec2 A, glm::vec2 B, glm::vec2 C, glm::vec2 D, float &t);

int solve2x2(float a11, float a12, float b1, float a21, float a22, float b2, float& x, float& y);

float cross2D (glm::vec2 a, glm::vec2 b) ;

bool pnpoly(const std::vector<glm::vec2>& points, glm::vec2 P);

glm::vec2 rot90(glm::vec2 in, bool clockwise = false);

std::vector<glm::vec2> DouglasPeucker(std::vector<glm::vec2>& pointList, float epsilon, int i0, int i1) ;
