#ifndef COLOR_H
#define COLOR_H

#include <fstream>
#include "../math/vec3.h"
#include "../math/interval.h"

using color = vec3;

void write_color(std::ofstream& file, const color& col) {
    // Red from left to right
    // Green from top to bottom
    const interval intensity{ 0.0, 1.0 };
    int ir = (int)(intensity.clamp(col.x) * 255.999);
    int ig = (int)(intensity.clamp(col.y) * 255.999);
    int ib = (int)(intensity.clamp(col.z) * 255.999);

    file << ir << " " << ig << " " << ib << " ";
}

#endif

