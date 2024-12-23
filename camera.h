#ifndef CAMERA_H
#define CAMERA_H

#include "entity.h"
#include "util/color.h"
#include "material.h"

inline std::tm localtime_xp(std::time_t timer) {
    std::tm bt{};
#if defined(__unix__)
    localtime_r(&timer, &bt);
#elif defined(_MSC_VER)
    localtime_s(&bt, &timer);
#else
    static std::mutex mtx;
    std::lock_guard<std::mutex> lock(mtx);
    bt = *std::localtime(&timer);
#endif
    return bt;
}

// "YYYY-MM-DD__HH-MM-SS"
inline std::string time_stamp(const std::string& fmt = "%F__%H-%M-%S") {
    auto bt = localtime_xp(std::time(0));
    char buf[64];
    return { buf, std::strftime(buf, sizeof(buf), fmt.c_str(), &bt) };
}

class camera {
    int image_width;
    point3 center;
    point3 pixel_center00;
    vec3 pixel_x;
    vec3 pixel_y;

	void initialize() {
        image_width = (int)(image_height * aspect_ratio);
        image_width = (image_width < 1) ? 1 : image_width; // Clamping

        center = point3(0, 0, 0);

        float focal_length = 1.0;
        float viewport_height = 2.0;
        float viewport_width = viewport_height * (float(image_width) / image_height); // Adjusted ratio

        vec3 viewport_x = vec3(viewport_width, 0, 0);
        vec3 viewport_y = vec3(0, -viewport_height, 0);
        pixel_x = viewport_x / image_width;
        pixel_y = viewport_y / image_height;

        point3 viewport_top_left = center - vec3(0, 0, focal_length) - viewport_x / 2 - viewport_y / 2;
        pixel_center00 = viewport_top_left + (pixel_x + pixel_y) / 2;

        std::clog << "image width: " << image_width << " image height: " << image_height << " viewport width: " << viewport_width << " viewport height: " << viewport_height << std::endl;
        std::clog << "viewport top left: " << viewport_top_left << " pixel_center00: " << pixel_center00 << " pixel size: " << pixel_x << " " << pixel_y << std::endl;
        /*for (int i = 0; i < 100; i++) {
            vec3 unit = rand_unit_vector();
            std::cout << unit << " " << unit.length_squared() << std::endl;
        } */
	}
	
    ray get_ray(int i, int j) const {
        vec3 offset{ randf() - 0.5f, randf() - 0.5f, 0 }; // Random point from center of unit square -0.5 <= x, y < 0.5
        point3 sample = pixel_center00 + ((i + offset.y) * pixel_y) + ((j + offset.x) * pixel_x);
        return ray{ center, sample - center };
    }

    color ray_color(const ray& r, const entity& world, int depth) {
        // Max recursion depth reached
        if (depth >= max_depth) {
            return color(0, 0, 0);
        }

        hit_result res;

        if (world.hit(r, interval(0.0001, infinity), res)) {
            ray scattered;
            color attenuation;
            if (res.mat->scatter(r, res, attenuation, scattered)) {
                return attenuation * ray_color(scattered, world, depth + 1);
            }

            return color(0, 0, 0); // absorbed
        }

        vec3 unit = unit_vector(r.dir);
        float a = 0.5 * (unit.y + 1.0); // Transform -1 < y < 1 to 0 < a < 1
        return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
    }
public:
    float aspect_ratio = 1.0;
    int image_height = 100;
    int pixel_samples = 10; // # of random samples for pixel (can be parallel reduced)
    int max_depth = 10; // Max # of ray bounces per ray

	void render(const entity& world) {
        initialize();

        std::ofstream file{ "renders/render_" + time_stamp() + ".ppm", std::ios::app };

        clock_t t0 = clock();
        file << "P3\n" << image_width << " " << image_height << "\n255\n";

        for (int i = 0; i < image_height; i++) {
            std::clog << "\rScanlines remaining: " << (image_height - i) << "     " << std::flush;
            for (int j = 0; j < image_width; j++) {
                color pixel_col(0, 0, 0);
                // Maybe sample only when near the edge?
                for (int s = 0; s < pixel_samples; s++) {
                    ray r = get_ray(i, j);
                    pixel_col += ray_color(r, world, 0);
                }
                write_color(file, pixel_col / pixel_samples);
            }
        }

        std::clog << "\rDone " << clock() - t0 << "ms                                                    \n";

        //std::clog << cross(vec3{ 5, -4, 12 }, vec3{ -16, -5, 2 }) << std::endl;

        file.close();
	}
};

#endif