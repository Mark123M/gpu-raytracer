#include "math/vec3.h"
#include "entity.h"
#ifndef SHAPE_H
#define SHAPE_H

class shape {
public:
	virtual float area() const = 0;
	virtual vec3 sample(const hit_result& res, point2 u) const = 0;
	virtual float pdf() const = 0;
};

class parallelogram : public shape {
	const vec3& u, v;
public:
	float area() const override {
		return cross(u, v).length();
	}

};

#endif // !SHAPE_H
