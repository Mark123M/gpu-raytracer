#ifndef LIGHT_H
#define LIGHT_H

#include "math/vec3.h"
#include "math/vec2.h"
#include "math/transform.h"
#include "util/color.h"

struct light_sample {

};

class light {
	int flags;
public:
	virtual light_sample sample_li(const point3& pi, const vec3& n, const point2& u, bool allow_incomplete_pdf) {
		return light_sample{};
	}

	virtual float pdf_li(const point3& pi, const vec3& n, const vec3& wi, bool allow_incomplete_pdf) {
		return 0.0;
	}

	virtual color le(const point3& p, const vec3& n, const point2& uv, const vec3& w) = 0;

	virtual power phi() {
		return { 0, 0, 0 };
	}
};

class area_light : public light {
	color col;
public:
	area_light(const color& col) : col{col} {}

	color le(const point3& p, const vec3& n, const point2& uv, const vec3& w) override {
		return col;
	}
};

#endif
