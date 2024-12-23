#ifndef UTIL_H
#define UTIL_H

#include <limits>
#include <random>

// Constants
const float infinity = std::numeric_limits<float>::infinity();
const float pi = 3.1415926535897932385;

// Utility functions
inline float deg2rad(float deg) {
	return deg * pi / 180.0;
}

static std::uniform_real_distribution<float> distribution(0.0, 1.0);
static std::mt19937 generator;

inline float randf() {
	return distribution(generator);
}
inline float randf(float min, float max) {
	return min + (max - min) * randf();
}

#endif

