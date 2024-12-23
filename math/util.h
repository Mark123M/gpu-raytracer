#ifndef UTIL_H
#define UTIL_H

#include <limits>
#include <random>

// Constants
const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

// Utility functions
inline double deg2rad(double deg) {
	return deg * pi / 180.0;
}

static std::uniform_real_distribution<double> distribution(0.0, 1.0);
static std::mt19937 generator;

inline double randf() {
	return distribution(generator);
}
inline double randf(double min, double max) {
	return min + (max - min) * randf();
}

#endif

