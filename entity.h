#ifndef ENTITY_H
#define ENTITY_H

#include "math/ray.h"
#include "math/interval.h"
#include "aabb.h"
#include "light.h"

const vec3 ZERO_VEC{ 0, 0, 0 };

class material; // For circular imports

// Use pointers to vector quantites as well to avoid copies
struct hit_result {
	point3 p;
	vec3 normal;
	transform m;
	std::shared_ptr<material> mat;
	std::shared_ptr<light> lig;
	float t;
	// Texture coordinates
	float u;
	float v;
	bool front_face;

	// Update local transform based on normal
	void update_transform() {
		vec3 right = normalize(vec3{ normal.z, 0, -normal.x });

		if (normal.x == 0 && normal.z == 0) {
			right = vec3{normal.y, 0, 0};
		}

		vec3 forward = cross(right, normal);
		m = transform{ right, normal, forward, ZERO_VEC };
	}

	void set_face_normal(const ray& r, const vec3& outward_normal) {
		front_face = dot(r.dir, outward_normal) < 0;
		normal = front_face ? outward_normal : -outward_normal;
		update_transform();
	}
};

class entity {
public:
	virtual ~entity() = default;
	virtual bool hit(const ray& r, interval ray_t, hit_result& res) const = 0;
	virtual const aabb& get_aabb() const = 0;
};

class translate : public entity {
	std::shared_ptr<entity> object;
	vec3 offset;
	aabb bbox;
public:
	translate(std::shared_ptr<entity> object, const vec3& offset)
		: object(object), offset(offset) {
		bbox = object->get_aabb() + offset;
	}
	bool hit(const ray& r, interval ray_t, hit_result& res) const override {
		// Move the ray backwards by the offset
		ray offset_r(r.origin - offset, r.dir);

		// Determine whether an intersection exists along the offset ray (and if so, where)
		if (!object->hit(offset_r, ray_t, res))
			return false;

		// Move the intersection point forwards by the offset
		res.p += offset;

		return true;
	}
	const aabb& get_aabb() const override { return bbox; }
};

class rotate_y : public entity {
	std::shared_ptr<entity> object;
	double sin_theta;
	double cos_theta;
	aabb bbox;
public:
	rotate_y(std::shared_ptr<entity> object, double angle) : object(object) {
		auto radians = deg2rad(angle);
		sin_theta = std::sin(radians);
		cos_theta = std::cos(radians);
		bbox = object->get_aabb();

		point3 min(infinity, infinity, infinity);
		point3 max(-infinity, -infinity, -infinity);

		for (int i = 0; i < 2; i++) {
			for (int j = 0; j < 2; j++) {
				for (int k = 0; k < 2; k++) {
					auto x = i * bbox.x.max + (1 - i) * bbox.x.min;
					auto y = j * bbox.y.max + (1 - j) * bbox.y.min;
					auto z = k * bbox.z.max + (1 - k) * bbox.z.min;

					auto newx = cos_theta * x + sin_theta * z;
					auto newz = -sin_theta * x + cos_theta * z;

					vec3 tester(newx, y, newz);

					for (int c = 0; c < 3; c++) {
						min.set(c, std::fmin(min.get(c), tester.get(c)));
						max.set(c, std::fmax(max.get(c), tester.get(c)));
					}
				}
			}
		}

		bbox = aabb(min, max);
	}
	bool hit(const ray& r, interval ray_t, hit_result& res) const override {

		// Transform the ray from world space to object space.

		auto origin = point3(
			(cos_theta * r.origin.x) - (sin_theta * r.origin.z),
			r.origin.y,
			(sin_theta * r.origin.x) + (cos_theta * r.origin.z)
		);

		auto direction = vec3(
			(cos_theta * r.dir.x) - (sin_theta * r.dir.z),
			r.dir.y,
			(sin_theta * r.dir.x) + (cos_theta * r.dir.z)
		);

		ray rotated_r(origin, direction);

		// Determine whether an intersection exists in object space (and if so, where).

		if (!object->hit(rotated_r, ray_t, res))
			return false;

		// Transform the intersection from object space back to world space.

		res.p = point3(
			(cos_theta * res.p.x) + (sin_theta * res.p.z),
			res.p.y,
			(-sin_theta * res.p.x) + (cos_theta * res.p.z)
		);

		res.normal = vec3(
			(cos_theta * res.normal.x) + (sin_theta * res.normal.z),
			res.normal.y,
			(-sin_theta * res.normal.x) + (cos_theta * res.normal.z)
		);

		res.update_transform();

		return true;
	}

	const aabb& get_aabb() const override { return bbox; }
};

#endif
