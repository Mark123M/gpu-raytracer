#ifndef MATERIAL_H
#define MATERIAL_H

#include "entity.h"
#include "util/color.h"

struct bsdf_sample {
	color f;
	vec3 wi;
	float pdf = 0;
	float eta = 1;
};

class material {
	int flags = 0;
	virtual spectrum f(const vec3& wo, const vec3& wi) const = 0;
	virtual bool sample_f(const vec3& wo, float uc, const point2& u2, bsdf_sample& bs) const = 0;
	virtual float pdf(const vec3& wo, const vec3& wi) const = 0;
public:
	virtual ~material() = default;

	spectrum f(const vec3& wo_world, const vec3& wi_world, const transform& m) const {
		vec3 wo = m.world_to_local(wo_world);
		vec3 wi = m.world_to_local(wi_world);
		return f(wo, wi);
	}

	bool sample_f(const vec3& wo_world, float uc, const point2& u2, const transform& m,  bsdf_sample& bs) const {
		vec3 wo = m.world_to_local(wo_world);

		if (!sample_f(wo, uc, u2, bs) || bs.f.near_zero() || !bs.pdf) {
			return false;
		}

		bs.wi = m.local_to_world(bs.wi);
		return true;
	}

	float pdf(const vec3& wo_world, const vec3& wi_world, const transform& m) const {
		vec3 wo = m.world_to_local(wo_world);
		vec3 wi = m.world_to_local(wi_world);
		return pdf(wo, wi);
	}

	bool same_hemisphere(const vec3& u, const vec3& v) const {
		return u.y * v.y >= 0;
	}

	// TODO: hemisphere to directional reflectance
	virtual spectrum phd(const vec3& wo_world, float c[], point2 u2[], const transform& m) const {
		return spectrum{ 0, 0, 0 };
	}

	virtual bool scatter(const ray& in, const hit_result& res, color& attenuation, ray& scattered) const {
		return false;
	}
};

class lambertian : public material {
	color albedo;

	spectrum f(const vec3& wo, const vec3& wi) const override {
		if (!same_hemisphere(wo, wi)) {
			return spectrum{ 0, 0, 0 };
		}

		return albedo * inv_pi;
	}

	bool sample_f(const vec3& wo, float uc, const point2& u2, bsdf_sample& bs) const override {
		vec3 wi = rand_cosine_hemisphere_vector(point2{randf(), randf()});
		float pdf = cosine_hemisphere_pdf(wi.y); // cos(theta) = wi.y
		
		bs.f = albedo * inv_pi;
		bs.wi = wi;
		bs.pdf = pdf;
		return true;
	}

	float pdf(const vec3& wo, const vec3& wi) const override {
		if (!same_hemisphere(wo, wi)) {
			return 0;
		}

		return cosine_hemisphere_pdf(wi.y);
	}

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

	spectrum f(const vec3& wo, const vec3& wi) const override {
		return { 0, 0, 0 };
	}

	bool sample_f(const vec3& wo, float uc, const point2& u2, bsdf_sample& bs) const override {
		return false;
	}

	float pdf(const vec3& wo, const vec3& wi) const override {
		return 0.0;
	}
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

	spectrum f(const vec3& wo, const vec3& wi) const override {
		return { 0, 0, 0 };
	}

	bool sample_f(const vec3& wo, float uc, const point2& u2, bsdf_sample& bs) const override {
		return false;
	}

	float pdf(const vec3& wo, const vec3& wi) const override {
		return 0.0;
	}
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