#ifndef INTERVAL_H
#define INTERVAL_H

#include "util.h"

class interval {
public:
	float min, max;

	interval(): min(+infinity), max(-infinity) {} // empty interval

	interval(float min, float max): min{min}, max{max} {}

	interval(const interval& a, const interval& b) {
		min = a.min <= b.min ? a.min : b.min;
		max = a.max >= b.max ? a.max : b.max;
	}

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

	interval expand(double delta) const {
		float padding = delta / 2;
		return interval(min - padding, max + padding);
	}
};

inline interval operator+(const interval& ival, double displacement) {
	return interval(ival.min + displacement, ival.max + displacement);
}

inline interval operator+(double displacement, const interval& ival) {
	return ival + displacement;
}

#endif