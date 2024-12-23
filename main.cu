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
    
    auto material_ground = std::make_shared<lambertian>(color(0.8, 0.8, 0.0));
    auto material_center = std::make_shared<lambertian>(color(0.1, 0.2, 0.5));
    auto material_left = std::make_shared<metal>(color(0.8, 0.8, 0.8), 0.3);
    auto material_right = std::make_shared<metal>(color(0.8, 0.6, 0.2), 1.0);

    world.objects.push_back(std::make_shared<sphere>(point3(0, -100.5, -1), 100, material_ground));
    world.objects.push_back(std::make_shared<sphere>(point3(0, 0, -1.2), 0.5, material_center));
    world.objects.push_back(std::make_shared<sphere>(point3(-1.0, 0, -1.0), 0.5, material_left));
    world.objects.push_back(std::make_shared<sphere>(point3(1.0, 0, -1.0), 0.5, material_right));

    camera cam;
    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_height = 225;
    cam.pixel_samples = 100;
    cam.max_depth = 50;
    cam.render(world);

    return 0;
}