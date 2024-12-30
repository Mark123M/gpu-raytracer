#ifndef BVH_H
#define BVH_H

#include <algorithm>
#include "math/util.h"
#include "aabb.h"
#include "entity.h"
#include "entitylist.h"

class bvh : public entity {
	std::shared_ptr<entity> left;
	std::shared_ptr<entity> right;
	aabb bbox;

	static bool box_x_compare(const std::shared_ptr<entity>& a, const std::shared_ptr<entity>& b) {
		return a->get_aabb().x.min < b->get_aabb().x.min;
	}
	static bool box_y_compare(const std::shared_ptr<entity>& a, const std::shared_ptr<entity>& b) {
		return a->get_aabb().y.min < b->get_aabb().y.min;
	}
	static bool box_z_compare(const std::shared_ptr<entity>& a, const std::shared_ptr<entity>& b) {
		return a->get_aabb().z.min < b->get_aabb().z.min;
	}
public:
	bvh(entity_list list) : bvh(list.objects, 0, list.objects.size()) {}

	bvh(std::vector<std::shared_ptr<entity>> & objects, size_t start, size_t end) {
		bbox = aabb(interval(infinity, -infinity), interval(infinity, -infinity), interval(infinity, -infinity)); // empty bbox

		for (size_t i = start; i < end; i++) {
			bbox = aabb(bbox, objects[i]->get_aabb());
		}

		int axis = bbox.longest_axis();
		size_t size = end - start;
		auto comparator = (axis == 0) ? box_x_compare : (axis == 1) ? box_y_compare : box_z_compare;

		if (size == 1) {
			left = right = objects[start];
		} else if (size == 2) {
			left = objects[start];
			right = objects[start + 1];
		} else {
			std::sort(objects.begin() + start, objects.begin() + end, comparator);
			int mid = start + size / 2;
			left = std::make_shared<bvh>(objects, start, mid); // constructs bvh from [start, mid)
			right = std::make_shared<bvh>(objects, mid, end); // constructs bvh from [mid, end)
		}
	}

	bool hit(const ray& r, interval ray_t, hit_result& res) const override {
		if (!bbox.hit(r, ray_t)) {
			return false;
		}

		bool hit_left = left->hit(r, ray_t, res);
		// Check if right node was hit first and update hit_result if so
		bool hit_right = right->hit(r, interval(ray_t.min, hit_left ? res.t : ray_t.max), res);

		return hit_left || hit_right;
	}

	const aabb& get_aabb() const override { return bbox; }
};

#endif