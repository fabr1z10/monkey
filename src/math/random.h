#pragma once

#include <random>
#include <memory>

class Random {
public:
	static Random& instance() {
		static Random instance; // Guaranteed to be destroyed.
		// Instantiated on first use.
		return instance;
	}
	int getUniform (int min, int max);
	float getUniformReal (float min, float max);
private:
	Random();
	std::unique_ptr<std::mt19937> m_gen;
};