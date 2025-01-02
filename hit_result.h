#ifndef HIT_RESULT_H
#define HIT_RESULT_H

#include "math/ray.h"
#include "math/transform.h"
#include "math/vec3.h"

class material;
class light;

const vec3 ZERO_VEC{ 0, 0, 0 };

// Use pointers to vector quantites as well to avoid copies
struct hit_result {
	point3 p;
	vec3 normal;
	transform m;
	std::shared_ptr<material> mat;
	std::shared_ptr<light> lig;
	float t;
	// Texture coordinates
	bool front_face;

	// Update local transform based on normal
	void update_transform() {
		vec3 right;

		if (normal.x == 0 && normal.z == 0) {
			right = vec3{ normal.y, 0, 0 };
		} else {
			right = normalize(vec3{ normal.z, 0, -normal.x });
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

#endif