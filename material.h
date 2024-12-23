#ifndef MATERIAL_H
#define MATERIAL_H

#include "entity.h"
#include "util/color.h"

class material {
public:
	virtual ~material() = default;

	virtual bool scatter(const ray& in, const hit_result& res, color& attenuation, ray& scattered) const {
		return false;
	}
};

class lambertian : public material {
	color albedo;
public:
	lambertian(const color& albedo): albedo{albedo} {}

	bool scatter(const ray& in, const hit_result& res, color& attenuation, ray& scattered) const override {
		vec3 scatter_dir = res.normal + rand_unit_vector();

		if (scatter_dir.near_zero()) {
			scatter_dir = res.normal;
		}

		scattered = ray(res.p, scatter_dir);
		attenuation = albedo;
		return true;
	}
};

class metal : public material {
	color albedo;
	float fuzz;
public:
	metal(const color& albedo, float fuzz) : albedo{ albedo }, fuzz{ fuzz < 1 ? fuzz : 1 } {}

	bool scatter(const ray& in, const hit_result& res, color& attenuation, ray& scattered) const override {
		vec3 reflected = reflect(in.dir, res.normal);
		reflected = unit_vector(reflected) + (fuzz * rand_unit_vector());
		scattered = ray(res.p, reflected);
		attenuation = albedo;
		return dot(reflected, res.normal) > 0; // < 0 means reflection went inside sphere, count as absorbed
	}
};

#endif