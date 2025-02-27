#ifndef UTIL_H
#define UTIL_H

#include <limits>
#include <random>

// Constants
constexpr float infinity = std::numeric_limits<float>::infinity();
constexpr float pi = 3.1415926535897932385;
constexpr float inv_pi = 0.31830988618379067154;
constexpr float sqrt2 = 1.41421356237309504880;
constexpr float pi_over_2 = 1.57079632679489661923;
constexpr float pi_over_4 = 0.78539816339744830961;

// Utility functions
inline float deg2rad(float deg) {
	return deg * pi / 180.0;
}

static std::random_device                  rand_dev;
static auto                                rng = std::default_random_engine(rand_dev());
static std::mt19937                        generator(rand_dev());
static std::uniform_real_distribution<float> distribution(0.0, 1.0);

inline float randf() {
	return distribution(generator);
}
inline float randf(float min, float max) {
	return min + (max - min) * randf();
}
inline int randi(int min, int max) {
	return (int)randf(min, max);
}

inline float power_heuristic(int ni, float pi, int nj, float pj) {
	float i = ni * pi;
	float j = nj * pj;
	return (i * i) / (i * i + j * j);
}

#endif

