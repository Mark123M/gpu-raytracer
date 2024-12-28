#ifndef AABB_H
#define AABB_H

#include "math/interval.h"
#include "math/vec3.h"
#include "math/ray.h"

class aabb {
	void min_padding() {
		float min_p = 0.0001;
		if (x.size() < min_p) {
			x = x.expand(min_p);
		}
		if (y.size() < min_p) {
			y = y.expand(min_p);
		}
		if (z.size() < min_p) {
			z = z.expand(min_p);
		}
	}
public:
	interval x, y, z;

	aabb() {}

	aabb(const interval& x, const interval& y, const interval& z): x{x}, y{y}, z{z} {
		min_padding();
	}
	
	aabb(const point3& a, const point3& b) {
		x = (a.x <= b.x) ? interval(a.x, b.x) : interval(b.x, a.x);
		y = (a.y <= b.y) ? interval(a.y, b.y) : interval(b.y, a.y);
		z = (a.z <= b.z) ? interval(a.z, b.z) : interval(b.z, a.z);
		min_padding();
	}

	aabb(const aabb& box0, const aabb& box1) {
		x = interval(box0.x, box1.x);
		y = interval(box0.y, box1.y);
		z = interval(box0.z, box1.z);
	}

	const interval& get_interval(int idx) const {
		if (idx == 0) {
			return x;
		} else if (idx == 1) {
			return y;
		}

		return z;
	}

	bool hit(const ray& r, interval t) const {
		for (int i = 0; i < 3; i++) {
			const interval& in = get_interval(i);
			const float dinv = 1.0 / r.dir.get(i);

			// Find the two 2 t's when r intersects min and max
			float t0 = (in.min - r.origin.get(i)) * dinv;
			float t1 = (in.max - r.origin.get(i)) * dinv;

			// Update t interval where all 3 intersections overlap
			if (t0 < t1) {
				if (t0 > t.min) t.min = t0;
				if (t1 < t.max) t.max = t1;
			} else {
				if (t1 > t.min) t.min = t1;
				if (t0 < t.max) t.max = t0;
			}

			// No interval where all 3 intersections overlap
			if (t.max <= t.min) {
				return false;
			}
		}

		return true;
	}

	int longest_axis() const {
		if (x.size() > y.size()) {
			return x.size() > z.size() ? 0 : 2;
		} else {
			return y.size() > z.size() ? 1 : 2;
		}
	}
};

#endif