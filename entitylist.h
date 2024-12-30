#ifndef ENTITY_LIST_H
#define ENTITY_LIST_H

#include <memory>
#include <vector>
#include "entity.h"
#include "aabb.h"

class entity_list : public entity {
	aabb bbox;
public:
	std::vector<std::shared_ptr<entity>> objects; // random index access

	entity_list() {}
	entity_list(std::shared_ptr<entity> object) { add(object); }
	
	void add(std::shared_ptr<entity> object) {
		objects.push_back(object);
		bbox = aabb(bbox, object->get_aabb());
	}

	void clear() { objects.clear(); }

	// Gets the first object the ray hits
	bool hit(const ray& r, interval ray_t, hit_result& res) const override {
		hit_result temp_res;
		float tmin_hit = ray_t.max;
		bool hit = false;

		// creating intervals might be sus...
		for (const std::shared_ptr<entity>& object : objects) {
			if (object->hit(r, interval(ray_t.min, tmin_hit), temp_res)) {
				hit = true;
				tmin_hit = temp_res.t;
				res = temp_res;
			}
		}

		return hit;
	}

	const aabb& get_aabb() const override { return bbox; }
};

#endif