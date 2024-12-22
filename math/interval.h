#ifndef INTERVAL_H
#define INTERVAL_H

#include "util.h"

class interval {
public:
	float min, max;

	interval(): min(+infinity), max(-infinity) {} // empty interval

	interval(float min, float max): min{min}, max{max} {}

	float size() const {
		return max - min; // inclusive?
	}

	bool contains(float x) const {
		return min <= x && x <= max;
	}

	bool contains_ex(float x) const {
		return min < x && x < max;
	}

	float clamp(float x) const {
		if (x < min) {
			return min;
		} else if (x > max) {
			return max;
		}

		return x;
	}
};

#endif