#ifndef SHAPE_H
#define SHAPE_H

#include "entity.h"
#include "hit_result.h"

class shape {
public:
	virtual float area() const = 0;
	virtual point3 sample(const hit_result& res, const point2& u) const = 0;
	// res_l is the hit_result from ray hitting the light
	virtual float pdf(const ray& r, const hit_result& res_l) const = 0;
};

class parallelogram : public shape {
	const point3& o;
	const vec3& u, v;
public:
	parallelogram(const point3& o, const vec3& u, const vec3& v): o{o}, u{u}, v{v} {}

	float area() const override {
		return cross(u, v).length();
	}

	point3 sample(const hit_result& res, const point2& u2) const override {
		return o + u2.x * u + u2.y * v;
	}

	float pdf(const ray& r, const hit_result& res_l) const override {
		float jacobian = dot(-r.dir, res_l.normal) / (r.origin - res_l.p).length_squared();
		return (1 / area()) / jacobian; // need to be converted to solid angle
	}
};

#endif // !SHAPE_H
