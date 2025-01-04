#ifndef RAY_H
#define RAY_H

#include "vec3.h"

class entity;

class ray {
public:
	point3 origin;
	vec3 dir;

	ray() : origin{ 0, 0, 0 }, dir{ 1, 0, 0 } {}
	ray(const point3& origin, const vec3& direction) : origin{ origin }, dir{ direction } {}
	ray(const point3& origin, const vec3& direction, const entity* target) : origin{ origin }, dir{ direction } {}

	point3 at(float t) const {
		return origin + t * dir;
	}
};

#endif