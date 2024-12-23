#ifndef INTERVAL_H
#define INTERVAL_H

#include "util.h"

class interval {
public:
	double min, max;

	interval(): min(+infinity), max(-infinity) {} // empty interval

	interval(double min, double max): min{min}, max{max} {}

	double size() const {
		return max - min; // inclusive?
	}

	bool contains(double x) const {
		return min <= x && x <= max;
	}

	bool contains_ex(double x) const {
		return min < x && x < max;
	}

	double clamp(double x) const {
		if (x < min) {
			return min;
		} else if (x > max) {
			return max;
		}

		return x;
	}
};

#endif