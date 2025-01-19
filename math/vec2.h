#ifndef VEC2_H
#define VEC2_H

#include <cmath>
#include <iostream>
#include <cassert>

class vec2;
inline vec2 operator*(float k, const vec2& v);
inline float dot(const vec2& a, const vec2& b);

class vec2 {
public:
	float x, y;

	vec2() : x{ 0 }, y{ 0 } {}

	vec2(float x0, float y0) : x{ x0 }, y{ y0 } {}

	vec2 operator-() const { return { -x, -y}; }

	vec2 operator+(const vec2& v) const { return { x + v.x, y + v.y}; }

	vec2 operator-(const vec2& v) const { return { x - v.x, y - v.y}; }

	vec2 operator*(const vec2& v) const { return { x * v.x, y * v.y}; }

	vec2 operator/(float k) const {
		assert(k != 0);
		return (1 / k) * (*this);
	}

	vec2& operator+=(const vec2& v) {
		x += v.x;
		y += v.y;
		return *this;
	}

	vec2& operator-=(const vec2& v) {
		x -= v.x;
		y -= v.y;
		return *this;
	}

	vec2& operator*=(float k) {
		x *= k;
		y *= k;
		return *this;
	}

	vec2& operator/=(float k) {
		assert(k != 0);
		x /= k;
		y /= k;
		return *this;
	}

	float get(int idx) const {
		if (idx == 0) {
			return x;
		}

		return y;
	}

	void set(int idx, float val) {
		if (idx == 0) {
			x = val;
		} else if (idx == 1) {
			y = val;
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
		return std::fabs(x) < t && std::fabs(y) < t;
	}
};

using point2 = vec2;

inline vec2 operator*(float k, const vec2& v) { return { k * v.x, k * v.y}; }

inline vec2 operator*(const vec2& v, float k) { return k * v; }

inline std::ostream& operator<<(std::ostream& out, const vec2& v) {
	out << v.x << " " << v.y;
	return out;
}

inline vec2 normalize(const vec2& v) {
	return v / v.length();
}

inline float dot(const vec2& a, const vec2& b) {
	return a.x * b.x + a.y * b.y;
}

#endif