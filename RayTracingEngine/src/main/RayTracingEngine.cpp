// Standard includes
#include <iostream>

// Formating
#include <iomanip>
#include <chrono>

// Own Files
#include "../utils/rtweekend.h"

#include "../utils/color.h"
#include "../objects/hittableList.h"
#include "../objects/sphere.h"
#include "../utils/camera.h"
#include "../texturing/material.h"
#include <vector>
#include <thread>
#include <string>
#include <sstream>
#include <future>
#include "../objects/movingSphere.h"
#include "../performance/bvh.h"
#include "../objects/aarect.h"
#include "../objects/box.h"
#include "../objects/constantMedium.h"
#include "application.h"


hittableList randomScene();

// Image

static int imageHeight = 0;
static int imageWidth = 0;
static int samplesPerPixel = 0;
static int maxDepth = 0;

hittableList randomScene();
hittableList twoSpheres();
hittableList twoPerlinSpheres();
hittableList earth();
hittableList simpleLight();
hittableList cornellBox();
hittableList finalScene();

// World

auto aspectRatio = 16.0 / 9.0;
auto background = color(0.70, 0.80, 1.00);
auto world = randomScene();
point3 lookfrom = point3(13, 2, 3);
point3 lookat = point3(0, 0, 0);
auto vfov = 20.0;
auto aperture = 0.1;

//auto aspectRatio = 16.0 / 9.0;
//auto background = color(0.70, 0.80, 1.00);
//auto world = twoSpheres();
//point3 lookfrom = point3(13, 2, 3);
//point3 lookat = point3(0, 0, 0);
//auto vfov = 20.0;
//auto aperture = 0.0;



//auto aspectRatio = 16.0 / 9.0;
//auto background = color(0.70, 0.80, 1.00);
//auto world = twoPerlinSpheres();
//auto lookfrom = point3(13, 2, 3);
//auto lookat = point3(0, 0, 0);
//auto vfov = 20.0;
//auto aperture = 0.0;


//auto aspectRatio = 16.0 / 9.0;
//auto background = color(0.70, 0.80, 1.00);
//auto world = earth();
//auto lookfrom = point3(13, 2, 3);
//auto lookat = point3(0, 0, 0);
//auto vfov = 20.0;
//auto aperture = 0.0;


//auto aspectRatio = 16.0 / 9.0;
//auto background = color(0, 0, 0);
//auto world = simpleLight();
//auto lookfrom = point3(26, 3, 6);
//auto lookat = point3(0, 2, 0);
//auto vfov = 20.0;
//auto aperture = 0.0;


//auto background = color(0, 0, 0);
//auto world = cornellBox();
//auto aspectRatio = 1.0;
//auto lookfrom = point3(278, 278, -800);
//auto lookat = point3(278, 278, 0);
//auto vfov = 40.0;
//auto aperture = 0.0;

//auto world = finalScene();
//auto aspectRatio = 1.0;
//auto background = color(0, 0, 0);
//auto lookfrom = point3(478, 278, -600);
//auto lookat = point3(278, 278, 0);
//auto vfov = 40.0;
//auto aperture = 0.0;

// Camera

vec3 vup(0, 1, 0);
auto distToFocus = 10.0;

camera cam(lookfrom, lookat, vup, vfov, aspectRatio, aperture, distToFocus, 0.0, 1.0);

color rayColor(const ray& r, const color& background, const hittable& world, int depth);

/*
void renderLine(int line) {
	for (int x = 0; x < imageWidth; ++x)
	{
		color pixel_color(0, 0, 0);
		for (int s = 0; s < samplesPerPixel; ++s) {
			auto u = (x + randomDouble()) / (imageWidth - 1);
			auto v = (line + randomDouble()) / (imageHeight - 1);
			ray r = cam.get_ray(u, v);
			pixel_color += rayColor(r, background, world, maxDepth);
		}
		writeColor(std::cout, pixel_color, samplesPerPixel);
	}
}
*/

/*
void renderSingleThread() {
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

	std::cout << "P3\n" << imageWidth << ' ' << imageHeight << "\n255\n";

	for (int y = imageHeight - 1; y >= 0; --y)
	{
		std::cerr << "Lines remaining: " << std::setw(3) << y << "\r";
		renderLine(y);
	}
	std::cerr << "\nDone";
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

	std::cerr << "Time difference = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() / 1000000.0 << "[s]" << std::endl;
	std::cerr << "Time difference = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() / 1000000.0 / 60 << "[min]" << std::endl;
}
*/

int main(int argc, char* argv[]) {
	application app = application();

	app.load(getRandomScene());

	app.start(true);

	app.output("test.ppm");

	std::cout << "\nPress any button to continue!";
	std::cin.ignore();

	return 0;
}

/*
color rayColor(const ray& r, const color& background, const hittable& world, int depth) {
	hitRecord rec;

	// If we've exceeded the ray bounce limit, no more light is gathered.
	if (depth <= 0)
		return color(0, 0, 0);

	// If the ray hits nothing, return the background color.
	if (!world.hit(r, 0.001, infinity, rec))
		return background;

	ray scattered;
	color attenuation;
	color emitted = rec.matPtr->emitted(rec.u, rec.v, rec.p);

	if (!rec.matPtr->scatter(r, rec, attenuation, scattered))
		return emitted;

	return emitted + attenuation * rayColor(scattered, background, world, depth - 1);
}
*/

//hittableList cornellBox() {
//	hittableList objects;
//
//	auto red = make_shared<lambertian>(color(.65, .05, .05));
//	auto white = make_shared<lambertian>(color(.73, .73, .73));
//	auto green = make_shared<lambertian>(color(.12, .45, .15));
//	auto light = make_shared<diffuseLight>(color(7, 7, 7));
//
//	objects.add(make_shared<yzRect>(0, 555, 0, 555, 555, green));
//	objects.add(make_shared<yzRect>(0, 555, 0, 555, 0, red));
//	objects.add(make_shared<xzRect>(113, 443, 127, 432, 554, light));
//	objects.add(make_shared<xzRect>(0, 555, 0, 555, 555, white));
//	objects.add(make_shared<xzRect>(0, 555, 0, 555, 0, white));
//	objects.add(make_shared<xyRect>(0, 555, 0, 555, 555, white));
//
//	shared_ptr<hittable> box1 = make_shared<box>(point3(0, 0, 0), point3(165, 330, 165), white);
//	box1 = make_shared<rotateY>(box1, 15);
//	box1 = make_shared<translate>(box1, vec3(265, 0, 295));
//
//	shared_ptr<hittable> box2 = make_shared<box>(point3(0, 0, 0), point3(165, 165, 165), white);
//	box2 = make_shared<rotateY>(box2, -18);
//	box2 = make_shared<translate>(box2, vec3(130, 0, 65));
//
//	objects.add(make_shared<constantMedium>(box1, 0.01, color(0, 0, 0)));
//	objects.add(make_shared<constantMedium>(box2, 0.01, color(1, 1, 1)));
//
//	return objects;
//}
//
//hittableList simpleLight() {
//	hittableList objects;
//
//	auto pertext = make_shared<noiseTexture>(4);
//	objects.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(pertext)));
//	objects.add(make_shared<sphere>(point3(0, 2, 0), 2, make_shared<lambertian>(pertext)));
//
//	auto difflight = make_shared<diffuseLight>(color(4, 4, 4));
//	objects.add(make_shared<xyRect>(3, 5, 1, 3, -2, difflight));
//
//	return objects;
//}
//
//hittableList earth() {
//	auto earthTexture = make_shared<imageTexture>("../../../RayTracingEngine/resources/earthmap.jpg");
//	auto earthSurface = make_shared<lambertian>(earthTexture);
//	auto globe = make_shared<sphere>(point3(0, 0, 0), 2, earthSurface);
//
//	return hittableList(globe);
//}
//
//hittableList twoSpheres() {
//	hittableList objects;
//
//	auto checker = make_shared<checkerTexture>(color(0.2, 0.3, 0.1), color(0.9, 0.9, 0.9));
//
//	objects.add(make_shared<sphere>(point3(0, -10, 0), 10, make_shared<lambertian>(checker)));
//	objects.add(make_shared<sphere>(point3(0, 10, 0), 10, make_shared<lambertian>(checker)));
//
//	return objects;
//}
//
//hittableList twoPerlinSpheres() {
//	hittableList objects;
//
//	auto pertext = make_shared<noiseTexture>(4);
//	objects.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(pertext)));
//	objects.add(make_shared<sphere>(point3(0, 2, 0), 2, make_shared<lambertian>(pertext)));
//
//	return objects;
//}