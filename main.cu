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
#include "quad.h"
#include "camera.h"
#include "bvh.h"

__global__ void hello() {
    printf("Hello from block: %u, thread: %u\n", blockIdx.x, threadIdx.x);
}

void spheres() {
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
    world.add(std::make_shared<sphere>(point3(0, -1000, 0), 1000, ground_material));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            float choose_mat = randf();
            point3 center(a + 0.9 * randf(), 0.2, b + 0.9 * randf());

            if ((center - point3(4, 0.2, 0)).length() > 0.9) {
                std::shared_ptr<material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = color{ randf(), randf(), randf() } *color{ randf(), randf(), randf() };
                    sphere_material = std::make_shared<lambertian>(albedo);
                    world.add(std::make_shared<sphere>(center, 0.2, sphere_material));
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = color{ randf(0.5, 1), randf(0.5, 1), randf(0.5, 1) };
                    auto fuzz = randf(0, 0.5);
                    sphere_material = std::make_shared<metal>(albedo, fuzz);
                    world.add(std::make_shared<sphere>(center, 0.2, sphere_material));
                } else {
                    // glass
                    sphere_material = std::make_shared<dielectric>(1.5);
                    world.add(std::make_shared<sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = std::make_shared<dielectric>(1.5);
    world.add(std::make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

    auto material2 = std::make_shared<lambertian>(color(0.4, 0.2, 0.1));
    world.add(std::make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

    auto material3 = std::make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
    world.add(std::make_shared<sphere>(point3(4, 1, 0), 1.0, material1));

    world = entity_list(std::make_shared<bvh>(world));

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
}

void quads() {
    entity_list world;

    // Materials
    auto left_red = std::make_shared<lambertian>(color(1.0, 0.2, 0.2));
    auto back_green = std::make_shared<lambertian>(color(0.2, 1.0, 0.2));
    auto right_blue = std::make_shared<lambertian>(color(0.2, 0.2, 1.0));
    auto upper_orange = std::make_shared<lambertian>(color(1.0, 0.5, 0.0));
    auto lower_teal = std::make_shared<lambertian>(color(0.2, 0.8, 0.8));

    // Quads
    world.add(std::make_shared<quad>(point3(-3, -2, 5), vec3(0, 0, -4), vec3(0, 4, 0), left_red));
    world.add(std::make_shared<quad>(point3(-2, -2, 0), vec3(4, 0, 0), vec3(0, 4, 0), back_green));
    world.add(std::make_shared<quad>(point3(3, -2, 1), vec3(0, 0, 4), vec3(0, 4, 0), right_blue));
    world.add(std::make_shared<quad>(point3(-2, 3, 1), vec3(4, 0, 0), vec3(0, 0, 4), upper_orange));
    world.add(std::make_shared<quad>(point3(-2, -3, 5), vec3(4, 0, 0), vec3(0, 0, -4), lower_teal));

    camera cam;

    cam.aspect_ratio = 1.0;
    cam.image_height = 400;
    cam.pixel_samples = 100;
    cam.max_depth = 50;

    cam.vfov = 80;
    cam.origin = point3(0, 0, 9);
    cam.lookat = point3(0, 0, 0);
    cam.vup = vec3(0, 1, 0);

    cam.render(world);
    
}

void cornell_box() {
    entity_list world;

    auto red = std::make_shared<lambertian>(color(.65, .05, .05));
    auto white = std::make_shared<lambertian>(color(.73, .73, .73));
    auto green = std::make_shared<lambertian>(color(.12, .45, .15));
    auto light = std::make_shared<area_light> (color(15, 15, 15));

    world.add(std::make_shared<quad>(point3(555, 0, 0), vec3(0, 555, 0), vec3(0, 0, 555), green));
    world.add(std::make_shared<quad>(point3(0, 0, 0), vec3(0, 555, 0), vec3(0, 0, 555), red));
    world.add(std::make_shared<quad>(point3(343, 554, 332), vec3(-130, 0, 0), vec3(0, 0, -105), light));
    world.add(std::make_shared<quad>(point3(0, 0, 0), vec3(555, 0, 0), vec3(0, 0, 555), white));
    world.add(std::make_shared<quad>(point3(555, 555, 555), vec3(-555, 0, 0), vec3(0, 0, -555), white));
    world.add(std::make_shared<quad>(point3(0, 0, 555), vec3(555, 0, 0), vec3(0, 555, 0), white));

    std::shared_ptr<entity> box1 = box(point3(0, 0, 0), point3(165, 330, 165), white);
    box1 = std:: make_shared<rotate_y>(box1, 15);
    box1 = std::make_shared<translate>(box1, vec3(265, 0, 295));
    world.add(box1);

    std::shared_ptr<entity> box2 = box(point3(0, 0, 0), point3(165, 165, 165), white);
    box2 = std::make_shared<rotate_y>(box2, -18);
    box2 = std::make_shared<translate>(box2, vec3(130, 0, 65));
    world.add(box2);

    camera cam;

    cam.aspect_ratio = 1.0;
    cam.image_height = 600;
    cam.pixel_samples = 200;
    cam.max_depth = 50;

    cam.vfov = 40;
    cam.origin = point3(278, 278, -800);
    cam.lookat = point3(278, 278, 0);
    cam.vup = vec3(0, 1, 0);

    cam.render(world);
}

void light() {
    entity_list world;

    auto light_mat = std::make_shared<area_light>(color{5.0, 5.0, 5.0});
    auto sphere_mat = std::make_shared<lambertian>(color{ 0.96078, 0.67059, 0.72549 });
    auto ground_mat = std::make_shared<lambertian>(color{ 0.35686, 0.81176, 0.98431 });
    world.add(std::make_shared<sphere>(point3(0, -1000, 0), 1000, ground_mat));
    world.add(std::make_shared<sphere>(point3(0, 2, 0), 2, sphere_mat));
    world.add(std::make_shared<quad>(point3(3, 1, -2), vec3(2, 0, 0), vec3(0, 2, 0), light_mat));

    camera cam;

    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_height = 400;
    cam.pixel_samples = 100;
    cam.max_depth = 50;

    cam.vfov = 20;
    cam.origin = point3(26, 3, 6);
    cam.lookat = point3(0, 2, 0);
    cam.vup = vec3(0, 1, 0);

    cam.render(world);
}

int main() {
    //spheres();
    //quads();
    //light();
    cornell_box();
    return 0;
}