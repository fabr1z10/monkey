#pragma once


struct vec2 {
	vec2() : x(0.f), y(0.f) {}
	vec2(float x, float y) : x(x), y(y) {}
	float x, y;

};

struct vec3 {
	vec3() : x(0.f), y(0.f), z(0.f) {}
	vec3(float x, float y, float z) : x(x), y(y), z(z) {}
	float x, y, z;
};

struct vec4 {
	vec4() : x(0.f), y(0.f), z(0.f), w(0.f) {}
	vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
	float x, y, z, w;
};