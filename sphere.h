#ifndef SPHERE_H
#define SPHERE_H

#include "entity.h"
#include "math/vec3.h"

class sphere : public entity {
	point3 center;
	float radius;
    aabb bbox;
public:
    sphere(const point3& center, float radius, std::shared_ptr<material> mat) : entity{mat, nullptr}, center { center }, radius{ radius > 0 ? radius : 0 } {
        vec3 offset = vec3(radius, radius, radius);
        bbox = aabb(center - offset, center + offset);
    }

	bool hit(const ray& r, interval ray_t, hit_result& res) const override {
        vec3 originToCenter = center - r.origin;
        float a = r.dir.length_squared();
        float h = dot(r.dir, originToCenter);
        float c = originToCenter.length_squared() - radius * radius;

        float discriminant = h * h - a * c;
        if (discriminant < 0) {
            return false;
        }

        float root = (h - std::sqrt(discriminant)) / a;
        if (!ray_t.contains_ex(root)) {
            root = (h + std::sqrt(discriminant)) / a;
            if (!ray_t.contains_ex(root)) {
                return false;
            }
        }

        res.t = root;
        res.p = r.at(root);
        res.target = this;
        vec3 outward_normal = (res.p - center) / radius;
        res.set_face_normal(r, outward_normal);
        
        return true;
	}

    const aabb& get_aabb() const override { return bbox; }
};

#endif