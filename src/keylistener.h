#pragma once

#include "hashpair.h"
struct GLFWwindow;

struct KeyboardEvent {
	int key;
	//int scancode;
	int action;
	int mods;

	bool operator==(const KeyboardEvent &other) const
	{ return (key == other.key
			  //&& scancode == other.scancode
			  && action == other.action
			  && mods == other.mods);
	}
};



namespace std {
	template<>
	struct hash<KeyboardEvent> {
		inline size_t operator()(const KeyboardEvent& p) const {
			size_t seed = 0;
			::hash_combine(seed, p.key);
			//::hash_combine(seed, p.scancode);
			::hash_combine(seed, p.action);
			::hash_combine(seed, p.mods);
			return seed;
		}
	};
}


class KeyboardListener {
public:
	KeyboardListener();
	virtual~ KeyboardListener();
	virtual int keyCallback(GLFWwindow*, int key, int scancode, int action, int mods) = 0;
};