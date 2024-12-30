#ifndef LIGHT_H
#define LIGHT_H

#include "math/vec3.h"
#include "math/vec2.h"
#include "math/transform.h"
#include "util/color.h"

struct light_sample {

};

class light {
protected:
	int flags;
	transform m;
	color col;
public:
	light (const transform& m, const color& col) : m{m}, col{col} {}

	light (const color& col): m{}, col{col} {}

	virtual light_sample sample_li(const point3& pi, const vec3& n, const point2& u, bool allow_incomplete_pdf) const = 0;

	virtual float pdf_li(const point3& pi, const vec3& n, const vec3& wi, bool allow_incomplete_pdf) const = 0;

	// Light for ray intersection
	virtual color l(const point3& p, const vec3& n, const point2& uv, const vec3& w) const {
		return col;
	}

	// Light that is emitted
	virtual color le(const point3& p, const vec3& n, const point2& uv, const vec3& w) const {
		return col;
	}

	virtual power phi() const {
		return { 0, 0, 0 };
	}
};

class area_light : public light {
	//shape s; // area, sample, intersection, pdf
public:
	area_light(const color& col) : light{col} {}
	area_light(const transform& m, const color& col) : light{m, col} {}
	
	light_sample sample_li(const point3& pi, const vec3& n, const point2& u, bool allow_incomplete_pdf) const override {
		return light_sample{};
	}

	// Light for ray intersection
	color l(const point3& p, const vec3& n, const point2& uv, const vec3& w) const override {
		return col;
	}

	float pdf_li(const point3& pi, const vec3& n, const vec3& wi, bool allow_incomplete_pdf) const override {
		return 0.0;
	}
};

#endif
