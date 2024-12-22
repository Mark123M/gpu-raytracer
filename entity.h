#ifndef ENTITY_H
#define ENTITY_H

#include "math/ray.h"
#include "math/interval.h"

struct hit_result {
	point3 p;
	vec3 normal;
	float t;
	bool front_face;

	void set_face_normal(const ray& r, const vec3& outward_normal) {
		front_face = dot(r.dir, outward_normal) < 0;
		normal = front_face ? outward_normal : -outward_normal;
	}
};

class entity {
public:
	virtual ~entity() = default;
	virtual bool hit(const ray& r, interval ray_t, hit_result& res) const = 0;
};

#endif
