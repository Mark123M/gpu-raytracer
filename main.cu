#include <stdio.h>
#include <iostream>
#include <fstream>

#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include "math/vec3.h"
#include "math/ray.h"
#include "math/util.h"
#include "util/color.h"
#include "entity.h"
#include "entitylist.h"
#include "material.h"
#include "sphere.h"
#include "camera.h"

__global__ void hello() {
    printf("Hello from block: %u, thread: %u\n", blockIdx.x, threadIdx.x);
}

int main() {
    entity_list world;
    
    /*auto material_ground = std::make_shared<lambertian>(color(0.8, 0.8, 0.0));
    auto material_center = std::make_shared<lambertian>(color(0.1, 0.2, 0.5));
    auto material_left = std::make_shared<dielectric>(1.50); // Air bubble in water //std::make_shared<metal>(color(0.8, 0.8, 0.8), 0.3);
    auto material_bubble = std::make_shared<dielectric>(1.00 / 1.50); // Material index over medium index (air in glass)
    auto material_right = std::make_shared<metal>(color(0.8, 0.6, 0.2), 1.0);

    world.objects.push_back(std::make_shared<sphere>(point3(0, -100.5, -1), 100, material_ground));
    world.objects.push_back(std::make_shared<sphere>(point3(0, 0, -1.2), 0.5, material_center));
    world.objects.push_back(std::make_shared<sphere>(point3(-1.0, 0, -1.0), 0.5, material_left));
    world.objects.push_back(std::make_shared<sphere>(point3(-1.0, 0, -1.0), 0.4, material_bubble));
    world.objects.push_back(std::make_shared<sphere>(point3(1.0, 0, -1.0), 0.5, material_right)); */

    auto ground_material = std::make_shared<lambertian>(color(0.5, 0.5, 0.5));
    world.objects.push_back(std::make_shared<sphere>(point3(0, -1000, 0), 1000, ground_material));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            float choose_mat = randf();
            point3 center(a + 0.9 * randf(), 0.2, b + 0.9 * randf());

            if ((center - point3(4, 0.2, 0)).length() > 0.9) {
                std::shared_ptr<material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = color{ randf(), randf(), randf() } * color{randf(), randf(), randf()};
                    sphere_material = std::make_shared<lambertian>(albedo);
                    world.objects.push_back(std::make_shared<sphere>(center, 0.2, sphere_material));
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = color{randf(0.5, 1), randf(0.5, 1), randf(0.5, 1)};
                    auto fuzz = randf(0, 0.5);
                    sphere_material = std::make_shared<metal>(albedo, fuzz);
                    world.objects.push_back(std::make_shared<sphere>(center, 0.2, sphere_material));
                } else {
                    // glass
                    sphere_material = std::make_shared<dielectric>(1.5);
                    world.objects.push_back(std::make_shared<sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = std::make_shared<dielectric>(1.5);
    world.objects.push_back(std::make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

    auto material2 = std::make_shared<lambertian>(color(0.4, 0.2, 0.1));
    world.objects.push_back(std::make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

    auto material3 = std::make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
    world.objects.push_back(std::make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

    camera cam;
    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_height = 675;
    cam.pixel_samples = 100;
    cam.max_depth = 20;

    cam.vfov = 20;
    cam.origin = point3(13, 2, 3);
    cam.lookat = point3(0, 0, 0);
    cam.vup = vec3(0, 1, 0);

    cam.render(world);
    return 0;
}