#ifndef UTIL_H
#define UTIL_H

#include <limits>
#include <random>

// Constants
const float infinity = std::numeric_limits<float>::infinity();
const float pi = 3.1415926535897932385;

// Utility functions
inline float deg_to_rad(float deg) {
	return deg * pi / 180.0;
}

inline float randf() {
    // Returns a random real in [0,1).
    return std::rand() / (RAND_MAX + 1.0);
}

inline float randf(float min, float max) {
    // Returns a random real in [min,max).
    return min + (max - min) * randf();
}

#endif

