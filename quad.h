#ifndef QUAD_H
#define QUAD_H

#include "entity.h"

class quad : public entity {
	point3 o;
	vec3 u, v;
	std::shared_ptr<material> mat;
	aabb bbox;
	vec3 normal;
	vec3 w; // Used to find local coordinates
	float D; // Ax + By + Cz = D
public:
	quad(const point3& o, const vec3& u, const vec3& v, std::shared_ptr<material> mat) : o{ o }, u{ u }, v{ v }, mat{ mat } {
		vec3 n = cross(u, v);
		normal = unit_vector(n);
		D = dot(normal, o);
		w = n / dot(n, n);

		set_bbox();
	}

	virtual void set_bbox() {
		aabb diag1 = aabb(o, o + u + v);
		aabb diag2 = aabb(o + u, o + v);
		bbox = aabb(diag1, diag2);
	}

	aabb get_aabb() const override { return bbox; }

	bool hit(const ray& r, interval ray_t, hit_result& res) const override {
		float denom = dot(normal, r.dir);

		// Parallel to plane
		if (std::fabs(denom) < 1e-8) {
			return false;
		}

		float t = (D - dot(normal, r.origin)) / denom;
		if (!ray_t.contains(t)) {
			return false;
		}

		// Find if hit point lies inside plane from it's local coordinates
		point3 p = r.at(t);
		float alpha = dot(w, cross(p - o, v));
		float beta = dot(w, cross(u, p - o));

		if (!is_interior(alpha, beta, res)) {
			return false;
		}

		res.t = t;
		res.p = p;
		res.mat = mat;
		res.set_face_normal(r, normal);

		return true;
	}

	virtual bool is_interior(float a, float b, hit_result& res) const {
		interval coord_bounds = interval(0, 1);

		if (!coord_bounds.contains(a) || !coord_bounds.contains(b)) {
			return false;
		}

		res.u = a;
		res.v = b;
		return true;
	}
};

#endif