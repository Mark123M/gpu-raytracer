#ifndef VEC3_H
#define VEC3_H

#include <cmath>
#include <iostream>
#include <cassert>
#include "util.h"
#include "vec2.h"

class vec3;
inline vec3 operator*(float k, const vec3& v);
inline float dot(const vec3& a, const vec3& b);

class vec3 {
public:
	float x, y, z;

	vec3(): x{0}, y{0}, z{0} {}

	vec3(float x0, float y0, float z0): x{x0}, y{y0}, z{z0} {}

	vec3 operator-() const { return { -x, -y, -z }; }

	vec3 operator+(const vec3& v) const { return { x + v.x, y + v.y, z + v.z }; }

	vec3 operator-(const vec3& v) const { return { x - v.x, y - v.y, z - v.z }; }

	vec3 operator*(const vec3& v) const { return { x * v.x, y * v.y, z * v.z }; }

	vec3 operator/(float k) const {
		assert(k != 0);
		return (1 / k) * (*this);
	}

	vec3& operator+=(const vec3& v) {
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	vec3& operator-=(const vec3& v) {
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}

	vec3& operator *=(const vec3& v) {
		x *= v.x;
		y *= v.y;
		z *= v.z;
		return *this;
	}

	vec3& operator*=(float k) {
		x *= k;
		y *= k;
		z *= k;
		return *this;
	}

	vec3& operator/=(float k) {
		assert(k != 0);
		x /= k;
		y /= k;
		z /= k;
		return *this;
	}

	float get(int idx) const {
		if (idx == 0) {
			return x;
		} else if (idx == 1) {
			return y;
		}

		return z;
	}

	void set(int idx, float val) {
		if (idx == 0) {
			x = val;
		} else if (idx == 1) {
			y = val;
		} else if (idx == 2) {
			z = val;
		}
	}

	float length() const {
		return std::sqrt(dot(*this, *this));
	}

	float length_squared() const {
		return dot(*this, *this);
	}

	bool near_zero() const {
		float t = 1e-8;
		return std::fabs(x) < t && std::fabs(y) < t && std::fabs(z) < t;
	}
};

using point3 = vec3;

inline vec3 operator*(float k, const vec3& v) { return {k * v.x, k * v.y, k * v.z}; }

inline vec3 operator*(const vec3& v, float k) { return k * v; }

inline std::ostream& operator<<(std::ostream& out, const vec3& v) {
	out << v.x << " " << v.y << " " << v.z;
	return out;
}

inline vec3 normalize(const vec3& v) {
	return v / v.length();
}

inline vec3 rand_unit_vector2() {
	while (true) {
		vec3 p = vec3{randf(-1, 1), randf(-1, 1), randf(-1, 1)};
		float lensq = p.length_squared();
		if (1e-80 < lensq && lensq <= 1) {
			return p / sqrt(lensq);
		}
	}
}

// Random vector in unit sphere
inline vec3 rand_unit_vector() {
	while (true) {
		vec3 p = vec3{ randf(-1, 1), randf(-1, 1), randf(-1, 1) };
		float lensq = p.length_squared();
		if (1e-80 < lensq && lensq <= 1) {
			return p / sqrt(lensq);
		}
	}
}

inline vec3 rand_hemisphere_vector(const vec3& normal) {
	vec3 unit = rand_unit_vector();
	if (dot(unit, normal) > 0) {
		return unit;
	} else {
		return -unit;
	}
}

inline vec3 rand_uniform_disk() {
	while (true) {
		auto p = vec3(randf(-1, 1), randf(-1, 1), 0);
		if (p.length_squared() < 1)
			return p;
	}
}

inline point2 rand_uniform_disk_concentric(const point2& u) {
	point2 u_offset = 2 * u - vec2(1, 1);
	if (u_offset.x == 0 && u_offset.y == 0) {
		return { 0, 0 };
	}

	float theta, r;
	if (std::abs(u_offset.x) > std::abs(u_offset.y)) {
		r = u_offset.x;
		theta = pi_over_4 * (u_offset.y / u_offset.x);
	} else {
		r = u_offset.y;
		theta = pi_over_2 - pi_over_4 * (u_offset.x / u_offset.y);
	}
	// Returns x and z coordinates on the horizontal plane
	return r * point2(std::cos(theta), std::sin(theta));
}

inline vec3 rand_cosine_hemisphere_vector(const point2& u) {
	point2 d = rand_uniform_disk_concentric(u);
	float y = std::sqrt(1 - d.x * d.x - d.y * d.y);
	return vec3{ d.x, y, d.y};
}

inline float cosine_hemisphere_pdf(float cos_theta) {
	return cos_theta * inv_pi;
}

inline vec3 reflect(const vec3& v, const vec3& n) {
	return v - 2 * dot(v, n) * n;
}

inline vec3 refract(const vec3& v, const vec3& n, float eta) {
	float cos_theta = std::fmin(dot(-v, n), 1.0);
	vec3 perp = eta * (v + cos_theta * n);
	vec3 parallel = -std::sqrt(std::fabs(1.0 - perp.length_squared())) * n;
	return perp + parallel;
}

inline float dot(const vec3& a, const vec3& b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline vec3 cross(const vec3& a, const vec3& b) {
	return vec3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}

#endif