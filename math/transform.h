#ifndef MAT4_H
#define MAT4_H

#include "vec3.h"

class transform {
public:
	float mat[3][4];
	float inv[3][4]; // inverse of mat, supports translation and rotation only

	transform(const vec3& right, const vec3& up, const vec3& forward, const vec3& translation): mat{
		{ right.x, up.x, forward.x, translation.x },
		{ right.y, up.y, forward.y, translation.y },
		{ right.z, up.z, forward.z, translation.z }}, inv{
		{ right.x,   right.y,   right.z,   -right.x * translation.x -   right.y * translation.y -   right.z * translation.z   },
		{ up.x,      up.y,      up.z,      -up.x * translation.x -      up.y * translation.y -      up.z * translation.z      },
		{ forward.x, forward.y, forward.z, -forward.x * translation.x - forward.y * translation.y - forward.z * translation.z }} {}

	vec3 apply(float m[4][4], const vec3& v) {
		vec3 r0{ m[0][0], m[0][1], m[0][2] };
		vec3 r1{ m[1][0], m[1][1], m[1][2] };
		vec3 r2{ m[2][0], m[2][1], m[2][2] };

		return { dot(r0, v) + m[0][3], dot(r1, v) + m[1][3], dot(r2, v) + m[2][3] };
	}
	
	vec3 local_to_world(const vec3& v) {
		return apply(mat, v);
	}

	vec3 world_to_local(const vec3& v) {
		return apply(inv, v);
	}
};

inline std::ostream& operator<<(std::ostream& out, const transform& t) {
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 4; j++) {
			out << t.mat[i][j] << " ";
		}
		out << std::endl;
	}

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 4; j++) {
			out << t.inv[i][j] << " ";
		}
		out << std::endl;
	}

	return out;
}

#endif