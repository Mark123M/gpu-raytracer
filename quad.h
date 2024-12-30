#ifndef QUAD_H
#define QUAD_H

#include "entity.h"

class quad : public entity {
	point3 o;
	vec3 u, v;
	std::shared_ptr<material> mat;
	std::shared_ptr<light> lig;
	aabb bbox;
	vec3 normal;
	vec3 w; // Used to find local coordinates
	float D; // Ax + By + Cz = D
public:
	quad(const point3& o, const vec3& u, const vec3& v, std::shared_ptr<material> mat, std::shared_ptr<light> lig) : o{ o }, u{ u }, v{ v }, mat{ mat }, lig{ lig } {
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

	const aabb& get_aabb() const override { return bbox; }

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
		// Need to include local transform matrix in light/mat
		res.mat = mat;
		res.lig = lig;
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

inline std::shared_ptr<entity_list> box(const point3& a, const point3& b, std::shared_ptr<material> mat) {
	// Returns the 3D box (six sides) that contains the two opposite vertices a & b.

	auto sides = std::make_shared<entity_list>();

	// Construct the two opposite vertices with the minimum and maximum coordinates.
	auto min = point3(std::fmin(a.x, b.x), std::fmin(a.y, b.y), std::fmin(a.z, b.z));
	auto max = point3(std::fmax(a.x, b.x), std::fmax(a.y, b.y), std::fmax(a.z, b.z));

	auto dx = vec3(max.x - min.x, 0, 0);
	auto dy = vec3(0, max.y - min.y, 0);
	auto dz = vec3(0, 0, max.z - min.z);

	sides->add(std::make_shared<quad>(point3(min.x, min.y, max.z), dx, dy, mat, nullptr)); // front
	sides->add(std::make_shared<quad>(point3(max.x, min.y, max.z), -dz, dy, mat, nullptr)); // right
	sides->add(std::make_shared<quad>(point3(max.x, min.y, min.z), -dx, dy, mat, nullptr)); // back
	sides->add(std::make_shared<quad>(point3(min.x, min.y, min.z), dz, dy, mat, nullptr)); // left
	sides->add(std::make_shared<quad>(point3(min.x, max.y, max.z), dx, -dz, mat, nullptr)); // top
	sides->add(std::make_shared<quad>(point3(min.x, min.y, min.z), dx, dz, mat, nullptr)); // bottom

	return sides;
}

#endif