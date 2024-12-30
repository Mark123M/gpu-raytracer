#ifndef CAMERA_H
#define CAMERA_H

#include "entity.h"
#include "util/color.h"
#include "material.h"
#include <sstream>

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
    point3 pixel_center00;
    vec3 pixel_x;
    vec3 pixel_y;
    vec3 x, y, z; // Local transform of the camera

	void initialize() {
        image_width = (int)(image_height * aspect_ratio);
        image_width = (image_width < 1) ? 1 : image_width; // Clamping

        

        float focal_length = (origin - lookat).length();
        float theta = deg2rad(vfov);
        float h = std::tan(theta / 2) * focal_length;
        float viewport_height = 2 * h;
        float viewport_width = viewport_height * (float(image_width) / image_height); // Adjusted ratio

        z = unit_vector(origin - lookat);
        x = unit_vector(cross(vup, z));
        y = cross(z, x);

        vec3 viewport_x = viewport_width * x;
        vec3 viewport_y = viewport_height * -y;
        pixel_x = viewport_x / image_width;
        pixel_y = viewport_y / image_height;

        point3 viewport_top_left = origin - focal_length * z - viewport_x / 2 - viewport_y / 2;
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
        return ray{ origin, sample - origin };
    }
    
    color ray_color(ray& r, const entity& world) {
        color L;
        spectrum beta{ 1, 1, 1 };

        int depth = 0;
        float p_b;
        float eta_scale = 1;

        bool specular_bounce = false;
        hit_result res;
        hit_result prev_res;

        while (true) {
            if (!world.hit(r, interval(0.001, infinity), res)) {
                // TODO: environment lights
                break;
            }

            if (res.lig != nullptr) {
                color Le = res.lig->le(res.p, res.normal, point2{ 0, 0 }, vec3{ 0, 0, 0 });
                if (depth == 0 || specular_bounce) {
                    L += beta * Le;
                } else {
                    // TODO: MIS weight
                    float p_l = 1.0;
                    float w_l = 1.0;
                    L += beta * w_l * Le;
                }
            }

            if (depth++ == max_depth) {
                break;
            }

            // TODO: direct illumination sampling
            if (res.mat != nullptr) {

            }

            vec3 wo = -r.dir;
            bsdf_sample bs;

            if (res.mat == nullptr) { // || !res.mat->sample_f(wo, randf(), point2{ randf(), randf() }, res.m, bs)) {
                break; // Maybe continue instead?
            }

            ray scattered;
            color attenuation;

            res.mat->scatter(r, res, attenuation, scattered);
            beta *= attenuation;
            prev_res = res;
            r = scattered;
        }

        return L;
    }
public:
    float aspect_ratio = 1.0;
    int image_height = 100;
    int pixel_samples = 10; // # of random samples for pixel (can be parallel reduced)
    int max_depth = 10; // Max # of ray bounces per ray

    float vfov = 90; // Vertical fov in degrees
    point3 origin = point3(0, 0, 0);
    point3 lookat = point3(0, 0, -1); // Point camera is looking at
    vec3 vup = vec3(0, 1, 0); // Up vector of camera

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
                    pixel_col += ray_color(r, world);
                }
                write_color(file, pixel_col / pixel_samples);
            }
        }

        int duration = clock() - t0;
        std::clog << "\rDone " << duration << "ms                                                    \n";
        
        std::stringstream ss;
        ss << duration/ 1000;
        std::string duration_string = ss.str();
        std::ofstream metadata{ "renders/render_" + time_stamp() + "_" + duration_string + "s.metadata.txt", std::ios::app };
        metadata << "Time elapsed: " << duration << " ms/" << duration / 1000.0 << " s/" << duration / 60000.0 << " m" << std::endl;

        file.close();
        metadata.close();
	}
};

#endif