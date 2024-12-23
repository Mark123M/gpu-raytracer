#ifndef COLOR_H
#define COLOR_H

#include <fstream>
#include "../math/vec3.h"
#include "../math/interval.h"

using color = vec3;

inline double apply_gamma(double f) {
    if (f > 0) {
        return std::sqrt(f);
    }
    return 0;
}

void write_color(std::ofstream& file, const color& col) {
    // Red from left to right
    // Green from top to bottom
    double r = apply_gamma(col.x);
    double g = apply_gamma(col.y);
    double b = apply_gamma(col.z);

    const interval intensity{ 0.0, 1.0 };
    int ir = (int)(intensity.clamp(r) * 255.999);
    int ig = (int)(intensity.clamp(g) * 255.999);
    int ib = (int)(intensity.clamp(b) * 255.999);

    file << ir << " " << ig << " " << ib << " ";
}

#endif

