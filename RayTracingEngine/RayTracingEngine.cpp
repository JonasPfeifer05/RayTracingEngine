// Standard includes
#include <iostream>

// Formating
#include <iomanip>
#include <chrono>

// Own Files
#include "rtweekend.h"

#include "color.h"
#include "hittable_list.h"
#include "sphere.h"
#include "camera.h"
#include "material.h"
#include <vector>
#include <thread>
#include <string>
#include <sstream>
#include <future>
#include"thread_pool.h"


hittable_list random_scene();

// Image

const static auto aspect_ratio = 3.0 / 2.0;
const static int image_width = 1200;
const static int image_height = static_cast<int>(image_width / aspect_ratio);
const static int samples_per_pixel = 500;
const static int max_depth = 50;

// World

static auto world = random_scene();

// Camera

static point3 lookfrom(13, 2, 3);
static point3 lookat(0, 0, 0);
static vec3 vup(0, 1, 0);
static auto dist_to_focus = 10.0;
static auto aperture = 0.02;

static camera cam(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus);


color ray_color(const ray& r, const hittable& world, int depth);
hittable_list random_scene();

void render_line(int line) {
    for (int x = 0; x < image_width; ++x)
    {
        color pixel_color(0, 0, 0);
        for (int s = 0; s < samples_per_pixel; ++s) {
            auto u = (x + random_double()) / (image_width - 1);
            auto v = (line + random_double()) / (image_height - 1);
            ray r = cam.get_ray(u, v);
            pixel_color += ray_color(r, world, max_depth);
        }
        write_color(std::cout, pixel_color, samples_per_pixel);
    }
}

void render_single_thread() {
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    for (int y = image_height - 1; y >= 0; --y)
    {
        std::cerr << "Lines remaining: " << std::setw(3) << y << "\r";
        render_line(y);
    }
    std::cerr << "\nDone";
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    std::cerr << "Time difference = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() / 1000000.0 << "[s]" << std::endl;
    std::cerr << "Time difference = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() / 1000000.0 / 60 << "[min]" << std::endl;
}

void test_pool() {
    ThreadPool pool;
    int sum = 0;
    int i;
    for (i = 0; i < 2000; i++)
    {
        pool.QueueJob(
            []() {
                for (size_t i = 0; i < 100000000; i++)
                {

                }
            }
        );
    }

    pool.Start();

    std::cerr << pool.busy();

    std::cerr << sum;
}

int main() {
    render_single_thread();
    //test_pool();

    return 0;
}

color ray_color(const ray& r, const hittable& world, int depth) {
    hit_record rec;

    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0)
        return color(0, 0, 0);

    if (world.hit(r, 0.001, infinity, rec)) {
        ray scattered;
        color attenuation;
        if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
            return attenuation * ray_color(scattered, world, depth - 1);
        return color(0, 0, 0);
    }
    vec3 unit_direction = unit_vector(r.direction());
    auto t = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - t) * color(1.0, 1.0, 1.0) + t * color(0.5, 0.7, 1.0);
}

hittable_list random_scene() {
    hittable_list world;

    auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground_material));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            point3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

            if ((center - point3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = color::random() * color::random();
                    sphere_material = make_shared<lambertian>(albedo);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
                else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = color::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
                else {
                    // glass
                    sphere_material = make_shared<dielectric>(1.5);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
    world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

    return world;
}