#ifndef CAMERA_H
#define CAMERA_H

#include "entity.h"
#include "light.h"
#include "util/color.h"
#include "material.h"
#include <sstream>
#include "math/util.h"

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

        z = normalize(origin - lookat);
        x = normalize(cross(vup, z));
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
    
    color sample_ld(ray& r, hit_result& res, const std::vector<entity*>& lights) {
        float u = randf();
        float p_light = 1.0 / lights.size();
        entity* sampled_light = nullptr;

        for (entity* e : lights) {
            u -= p_light;
            if (u < 0) {
                sampled_light = e;
            }
        }

        light_sample ls;
        if (sampled_light != nullptr && !sampled_light->lig->sample_li(res, ls, sampled_light)) {
            return { 0, 0, 0 };
        }

        vec3 wo = -r.dir;
        vec3 wi = ls.wi;
        std::shared_ptr<material> mat = res.target->mat;
        spectrum f = mat->f(wo, wi, res.m) * dot(wi, res.normal);
        float p_l = p_light * ls.pdf;

        if (sampled_light->lig->is_delta()) {
            return ls.L * f / p_l;
        } else {
            float p_b = mat->pdf(wo, wi, res.m);
            float w_l = power_heuristic(1, p_l, 1, p_b);
            return (w_l * ls.L * f) / p_l;
        }
    }

    color ray_color(ray& r, const entity& world, const std::vector<entity*>& lights) {
        color L;
        spectrum beta{ 1, 1, 1 };

        int depth = 0;
        float p_b = 0;
        float eta_scale = 1;

        bool specular_bounce = false;
        hit_result res;
        hit_result prev_res;

        while (true) {
            if (!world.hit(r, interval(0.001, infinity), res)) {
                // TODO: environment lights
                break;
            }

            std::shared_ptr<material> mat = res.target->mat;
            std::shared_ptr<light> lig = res.target->lig;

            if (lig != nullptr) {
                color Le = lig->Le(res.p, res.normal, vec3{ 0, 0, 0 });
                if (depth == 0 || specular_bounce) {
                    L += beta * Le;
                } else {
                    // Don't need shadow ray check because light was hit
                    float p_l = (1.0 / lights.size()) * lig->pdf_li(r, res);
                    float w_b = power_heuristic(1, p_b, 1, p_l);
                    L += beta * w_b * Le;
                }
            }

            if (depth++ == max_depth) {
                break;
            }

            // TODO: direct illumination sampling
            if (mat != nullptr) {
                color Ld = sample_ld(r, res, lights);
                L += beta * Ld;
            }

            vec3 wo = -r.dir;
            bsdf_sample bs;

            if (mat == nullptr || !mat->sample_f(wo, randf(), point2{ randf(), randf() }, res.m, bs)) {
                break; // Maybe continue instead?
            }

            beta *= (bs.f * dot(bs.wi, res.normal)) / bs.pdf;
            p_b = bs.pdf;
            prev_res = res;
            r = ray{res.p, bs.wi};

            float beta_max = std::max(beta.x, std::max(beta.y, beta.z));
            if (beta_max <= 1 && depth > 1) {
                float q = std::max(0.0f, 1 - beta_max);
                if (randf() < q) {
                    break;
                }
                beta /= 1 - q;
            }
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

	void render(const entity& world, const std::vector<entity*>& lights) {
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
                    pixel_col += ray_color(r, world, lights);
                }
                write_color(file, pixel_col / pixel_samples);
            }
        }
        /*ray test = get_ray(500, 170);
        ray_color(test, world, lights); */

        int duration = clock() - t0;
        std::clog << "\rDone " << duration << "ms                                                    \n";
        
        std::stringstream ss;
        ss << duration/ 1000;
        std::string duration_string = ss.str();
        std::ofstream metadata{ "renders/render_" + time_stamp() + "_" + duration_string + "s.metadata.txt", std::ios::app };
        metadata << "time elapsed: " << duration << " ms/" << duration / 1000.0 << " s/" << duration / 60000.0 << " m" << std::endl;
        metadata << "width: " << image_width << " height: " << image_height << "  " << std::endl;
        metadata << "samples: " << pixel_samples << " max depth: " << max_depth << std::endl;

        file.close();
        metadata.close();
	}
};

#endif