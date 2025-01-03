#ifndef LIGHT_H
#define LIGHT_H

#include "math/vec3.h"
#include "math/vec2.h"
#include "math/transform.h"
#include "util/color.h"
#include "shape.h"
#include "hit_result.h"
#include "entity.h"

struct light_sample {
	color L;
	vec3 wi;
	float pdf;
};

class light {
protected:
	int flags;
	transform m;
	color col;
	const entity& world;
public:
	light(const transform& m, const color& col, const entity& world) : m{ m }, col{ col }, world{ world } {}

	light(const color& col, const entity& world) : m{}, col{ col }, world{ world } {}

	virtual bool sample_li(hit_result& res, light_sample& ls, const entity* target, bool allow_incomplete_pdf = false) const = 0;

	virtual float pdf_li(const ray& r_shadow, const hit_result& res_l, bool allow_incomplete_pdf = false) const = 0;

	// Light for ray intersection
	virtual color L(const point3& p, const vec3& n, const vec3& w) const {
		return col;
	}

	// Light that is emitted
	virtual color Le(const point3& p, const vec3& n, const vec3& w) const {
		return col;
	}

	virtual power phi() const {
		return { 0, 0, 0 };
	}

	bool is_delta() const {
		return false;
	}
};

/*sample_li > sample point3 pl > calculate wi > calculate pdf > return L(p, -wi)
pdf is where light intersection is checked….My current system already assigns normals so this should work.

I need res to store point of intersection though to spawn the new ray */

class area_light : public light {
public:
	area_light(const color& col, const entity& world) : light{ col, world } {}

	area_light(const transform& m, const color& col, const entity& world) : light{ m, col, world } {}
	
	bool sample_li(hit_result& res, light_sample& ls, const entity* target, bool allow_incomplete_pdf = false) const override {
		// Sample point on shape
		point3 p_s = target->s->sample(res, point2{randf(), randf()});

		// Check if light is blocked or not
		vec3 wi = normalize(p_s - res.p);
		ray r_shadow{ res.p, wi };
		hit_result res_l;
		if (!world.hit(r_shadow, interval(0.001, infinity), res_l) || res_l.target != target) {
			return false;
		}
		
		// Compute radiance from light
		color Le = L(res_l.p, res_l.normal, -wi);
		float pdf_s = target->s->pdf(r_shadow, res_l);

		ls.L = Le;
		ls.wi = wi;
		ls.pdf = pdf_s;
		return true;
	}

	float pdf_li(const ray& r, const hit_result& res_l, bool allow_incomplete_pdf = false) const override {
		return res_l.target->s->pdf(r, res_l); // target->s->pdf(res, res_l, wi);
	}

	// Light for ray intersection
	color L(const point3& p, const vec3& n, const vec3& w) const override {
		return col;
	}
};

#endif