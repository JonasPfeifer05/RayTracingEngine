#include "../../utils/camera.h"
#include "../../utils/vec3.h"

camera randomCamera();
hittableList randomScene();

class scene

{
public:
	scene() : scene(720, 16.0/9.0, color(0.70, 0.80, 1.00), randomCamera(), randomScene()) {}
	scene(int height, double ratio, color background, camera cam, hittableList world) 
		: m_height(height), m_ratio(ratio), m_background(background), m_camera(cam), m_world(world) {}

	virtual int getHeigth() {
		return m_height;
	}

	virtual int getWidth() {
		return m_height * m_ratio;
	}

	int getLength() {
		return this->getHeigth() * this->getWidth();
	}

	virtual double getRatio() {
		return m_ratio;
	}

	virtual camera getCamera() {
		return m_camera;
	}

	virtual color getBackgroundColor() {
		return m_background;
	}

	virtual hittableList getWorld() {
		return m_world;
	}
private:
	int m_height;
	double m_ratio;
	camera m_camera;
	color m_background;
	hittableList m_world;
};

camera randomCamera() {
	return camera(point3(13, 2, 3), point3(0, 0, 0), vec3(0, 1, 0), 20.0, 16.0 / 9.0, 0.1, 10.0, 0.0, 1.0);
}

hittableList randomScene() {
	hittableList world;

	auto checker = make_shared<checkerTexture>(color(0.2, 0.3, 0.1), color(0.9, 0.9, 0.9));
	world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(checker)));

	hittableList balls;
	for (int a = -11; a < 11; a++) {
		for (int b = -11; b < 11; b++) {
			auto chooseMat = randomDouble();
			point3 center(a + 0.9 * randomDouble(), 0.2, b + 0.9 * randomDouble());

			if ((center - vec3(4, 0.2, 0)).length() > 0.9) {
				shared_ptr<material> sphereMaterial;

				if (chooseMat < 0.8) {
					// diffuse
					auto albedo = color::random() * color::random();
					sphereMaterial = make_shared<lambertian>(albedo);
					auto center2 = center + vec3(0, randomDouble(0, .5), 0);
					balls.add(make_shared<movingSphere>(
						center, center2, 0.0, 1.0, 0.2, sphereMaterial));
				}
				else if (chooseMat < 0.95) {
					// metal
					auto albedo = color::random(0.5, 1);
					auto fuzz = randomDouble(0, 0.5);
					sphereMaterial = make_shared<metal>(albedo, fuzz);
					balls.add(make_shared<sphere>(center, 0.2, sphereMaterial));
				}
				else {
					// glass
					sphereMaterial = make_shared<dielectric>(1.5);
					balls.add(make_shared<sphere>(center, 0.2, sphereMaterial));
				}
			}
		}
	}

	world.add(make_shared<bvhNode>(balls, 0, 1));

	auto material1 = make_shared<dielectric>(1.5);
	world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

	auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
	world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

	auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
	world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

	return world;
}

scene getRandomScene() {
	return scene();
}

camera finalCamera() {
	return camera(point3(478, 278, -600), point3(278, 278, 0), vec3(0, 1, 0), 40.0, 1.0, 0.0, 10, 0.0, 1.0);
}

hittableList finalScene() {
	hittableList boxes1;
	auto ground = make_shared<lambertian>(color(0.48, 0.83, 0.53));

	const int boxesPerSide = 20;
	for (int i = 0; i < boxesPerSide; i++) {
		for (int j = 0; j < boxesPerSide; j++) {
			auto w = 100.0;
			auto x0 = -1000.0 + i * w;
			auto z0 = -1000.0 + j * w;
			auto y0 = 0.0;
			auto x1 = x0 + w;
			auto y1 = randomDouble(1, 101);
			auto z1 = z0 + w;

			boxes1.add(make_shared<box>(point3(x0, y0, z0), point3(x1, y1, z1), ground));
		}
	}

	hittableList objects;

	objects.add(make_shared<bvhNode>(boxes1, 0, 1));

	auto light = make_shared<diffuseLight>(color(7, 7, 7));
	objects.add(make_shared<xzRect>(123, 423, 147, 412, 554, light));

	auto center1 = point3(400, 400, 200);
	auto center2 = center1 + vec3(30, 0, 0);
	auto movingSphereMaterial = make_shared<lambertian>(color(0.7, 0.3, 0.1));
	objects.add(make_shared<movingSphere>(center1, center2, 0, 1, 50, movingSphereMaterial));

	objects.add(make_shared<sphere>(point3(260, 150, 45), 50, make_shared<dielectric>(1.5)));
	objects.add(make_shared<sphere>(
		point3(0, 150, 145), 50, make_shared<metal>(color(0.8, 0.8, 0.9), 1.0)
		));

	auto boundary = make_shared<sphere>(point3(360, 150, 145), 70, make_shared<dielectric>(1.5));
	objects.add(boundary);
	objects.add(make_shared<constantMedium>(boundary, 0.2, color(0.2, 0.4, 0.9)));
	boundary = make_shared<sphere>(point3(0, 0, 0), 5000, make_shared<dielectric>(1.5));
	objects.add(make_shared<constantMedium>(boundary, .0001, color(1, 1, 1)));

	auto emat = make_shared<lambertian>(make_shared<imageTexture>("./RayTracingEngine/resources/earthmap.jpg"));
	objects.add(make_shared<sphere>(point3(400, 200, 400), 100, emat));
	auto pertext = make_shared<noiseTexture>(0.1);
	objects.add(make_shared<sphere>(point3(220, 280, 300), 80, make_shared<lambertian>(pertext)));

	hittableList boxes2;
	auto white = make_shared<lambertian>(color(.73, .73, .73));
	int ns = 1000;
	for (int j = 0; j < ns; j++) {
		boxes2.add(make_shared<sphere>(point3::random(0, 165), 10, white));
	}

	objects.add(make_shared<translate>(
		make_shared<rotateY>(
			make_shared<bvhNode>(boxes2, 0.0, 1.0), 15),
		vec3(-100, 270, 395)
		)
	);

	return objects;
}


scene getFinalScene() {
	return scene(720, 1.0, color(0, 0, 0), finalCamera(), finalScene());
}