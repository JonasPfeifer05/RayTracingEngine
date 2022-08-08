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
#include "moving_sphere.h"
#include "bvh.h"
#include "aarect.h"


hittable_list random_scene();

// Image

auto aspect_ratio = 16.0 / 9.0;
static int image_height = 0;
static int image_width = 0;
static int samples_per_pixel = 0;
static int max_depth = 0;

hittable_list random_scene();
hittable_list two_spheres();
hittable_list two_perlin_spheres();
hittable_list earth();
hittable_list simple_light();

// World
/*
    auto background = color(0.70, 0.80, 1.00);
    hittable_list world = random_scene();
    point3 lookfrom = point3(13, 2, 3);
    point3 lookat = point3(0, 0, 0);
    auto vfov = 20.0;
    auto aperture = 0.1;
  */  
    /*            
    auto background = color(0.70, 0.80, 1.00);
    hittable_list world = two_spheres();
    point3 lookfrom = point3(13, 2, 3);
    point3 lookat = point3(0, 0, 0);
    auto vfov = 20.0;
    auto aperture = 0.0;
    */

    /*
    auto background = color(0.70, 0.80, 1.00);
    auto world = two_perlin_spheres();
    auto lookfrom = point3(13, 2, 3);
    auto lookat = point3(0, 0, 0);
    auto vfov = 20.0;
    auto aperture = 0.0;
    */
    /*
    auto background = color(0.70, 0.80, 1.00);
    auto world = earth();
    auto lookfrom = point3(13, 2, 3);
    auto lookat = point3(0, 0, 0);
    auto vfov = 20.0;
    auto aperture = 0.0;
    */
    
    auto background = color(0, 0, 0);
    auto world = simple_light();
    auto lookfrom = point3(26, 3, 6);
    auto lookat = point3(0, 2, 0);
    auto vfov = 20.0;
    auto aperture = 0.0;


// Camera

vec3 vup(0, 1, 0);
auto dist_to_focus = 10.0;

camera cam(lookfrom, lookat, vup, vfov, aspect_ratio, aperture, dist_to_focus, 0.0, 1.0);

color ray_color(const ray& r, const color& background, const hittable& world, int depth);


void render_line(int line) {
    for (int x = 0; x < image_width; ++x)
    {
        color pixel_color(0, 0, 0);
        for (int s = 0; s < samples_per_pixel; ++s) {
            auto u = (x + random_double()) / (image_width - 1);
            auto v = (line + random_double()) / (image_height - 1);
            ray r = cam.get_ray(u, v);
            pixel_color += ray_color(r, background, world, max_depth);
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

int main() {
    std::cerr << "Enter Height of the image(like 144 240 360 480 720 1080): ";
    std::cin >> image_height;
    image_width = image_height * aspect_ratio;

    std::cerr << "Enter amount of Samples per Pixel: ";
    std::cin >> samples_per_pixel;

    std::cerr << "Enter amount of max Bounces per Ray: ";
    std::cin >> max_depth;

    camera cam(lookfrom, lookat, vup, vfov, aspect_ratio, aperture, dist_to_focus, 0.0, 1.0);

    render_single_thread();

    std::cerr << "FINISHED";
    //test_pool();

    return 0;
}

color ray_color(const ray& r, const color& background, const hittable& world, int depth) {
    hit_record rec;

    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0)
        return color(0, 0, 0);

    // If the ray hits nothing, return the background color.
    if (!world.hit(r, 0.001, infinity, rec))
        return background;

    ray scattered;
    color attenuation;
    color emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);

    if (!rec.mat_ptr->scatter(r, rec, attenuation, scattered))
        return emitted;

    return emitted + attenuation * ray_color(scattered, background, world, depth - 1);
}

hittable_list simple_light() {
    hittable_list objects;

    auto pertext = make_shared<noise_texture>(4);
    objects.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(pertext)));
    objects.add(make_shared<sphere>(point3(0, 2, 0), 2, make_shared<lambertian>(pertext)));

    auto difflight = make_shared<diffuse_light>(color(4, 4, 4));
    objects.add(make_shared<xy_rect>(3, 5, 1, 3, -2, difflight));

    return objects;
}

hittable_list earth() {
    auto earth_texture = make_shared<image_texture>("earthmap.jpg");
    auto earth_surface = make_shared<lambertian>(earth_texture);
    auto globe = make_shared<sphere>(point3(0, 0, 0), 2, earth_surface);

    return hittable_list(globe);
}

hittable_list two_spheres() {
    hittable_list objects;

    auto checker = make_shared<checker_texture>(color(0.2, 0.3, 0.1), color(0.9, 0.9, 0.9));

    objects.add(make_shared<sphere>(point3(0, -10, 0), 10, make_shared<lambertian>(checker)));
    objects.add(make_shared<sphere>(point3(0, 10, 0), 10, make_shared<lambertian>(checker)));

    return objects;
}

hittable_list two_perlin_spheres() {
    hittable_list objects;

    auto pertext = make_shared<noise_texture>(4);
    objects.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(pertext)));
    objects.add(make_shared<sphere>(point3(0, 2, 0), 2, make_shared<lambertian>(pertext)));

    return objects;
}

hittable_list random_scene() {
    hittable_list world;

    auto checker = make_shared<checker_texture>(color(0.2, 0.3, 0.1), color(0.9, 0.9, 0.9));
    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(checker)));

    hittable_list balls;
    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            point3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

            if ((center - vec3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = color::random() * color::random();
                    sphere_material = make_shared<lambertian>(albedo);
                    auto center2 = center + vec3(0, random_double(0, .5), 0);
                    balls.add(make_shared<moving_sphere>(
                        center, center2, 0.0, 1.0, 0.2, sphere_material));
                }
                else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = color::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                    balls.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
                else {
                    // glass
                    sphere_material = make_shared<dielectric>(1.5);
                    balls.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    world.add(make_shared<bvh_node>(balls, 0, 1));

    auto material1 = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
    world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

    return world;
}