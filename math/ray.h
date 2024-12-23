#ifndef RAY_H
#define RAY_H

#include "vec3.h"

class ray {
public:
	point3 origin;
	vec3 dir;

	ray() {}
	ray(const point3& origin, const vec3& direction): origin{origin}, dir{direction} {}

	point3 at(double t) const {
		return origin + t * dir;
	}
};

#endif