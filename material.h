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

class dielectric : public material {
	float index;
public:
	dielectric(float index): index{index} {}

	bool scatter(const ray& in, const hit_result& res, color& attenuation, ray& scattered) const override {
		attenuation = color(1.0, 1.0, 1.0);
		float ri = res.front_face ? (1.0 / index) : index; // index1 / index2

		vec3 unit = unit_vector(in.dir);
		float cos_theta = std::fmin(dot(-unit, res.normal), 1.0);
		float sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);

		if (ri * sin_theta > 1.0 || reflectance(cos_theta, ri) > randf()) { // Reflection: total internal reflection or viewing angle
			scattered = ray(res.p, reflect(unit, res.normal));
		} else {
			scattered = ray(res.p, refract(unit, res.normal, ri));
		}

		return true;
	}

	static float reflectance(float cosine, float ri) {
		// Shlick's approximation for reflectance
		float r0 = (1 - ri) / (1 + ri);
		r0 *= r0;
		return r0 + (1 - r0) * std::pow((1 - cosine), 5);
	}
};

#endif