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
#include "sphere.h"
#include "camera.h"

__global__ void hello() {
    printf("Hello from block: %u, thread: %u\n", blockIdx.x, threadIdx.x);
}

int main() {
    entity_list world;
    world.objects.push_back(std::make_shared<sphere>(point3(0, 0, -1), 0.5));
    world.objects.push_back(std::make_shared<sphere>(point3(0, -100.5, -1), 100));

    camera cam;
    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_height = 225;
    cam.pixel_samples = 100;
    cam.max_depth = 50;
    cam.render(world);

    return 0;
}