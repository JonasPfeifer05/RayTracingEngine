class scene
{
public:
	scene();
	~scene();

	virtual int getHeigth() {
		return 720;
	}

	virtual int getWidth() {
		return this->getHeigth() * this->getRatio();
	}

	int getLength() {
		return this->getHeigth() * this->getWidth();
	}

	virtual double getRatio() {
		return 16.0 / 9.0;
	}

	virtual camera* getCamera() {
		return new camera(point3(13, 2, 3), point3(0, 0, 0), vec3(0,1,0), 20.0, this->getRatio(), 0.1, 10.0, 0.0, 1.0);
	}

	virtual color getBackgroundColor() {
		return color(0.70, 0.80, 1.00);
	}

	virtual hittableList* getWorld() {
		hittableList* world = new hittableList();

		auto checker = make_shared<checkerTexture>(color(0.2, 0.3, 0.1), color(0.9, 0.9, 0.9));
		world->add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(checker)));

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

		world->add(make_shared<bvhNode>(balls, 0, 1));

		auto material1 = make_shared<dielectric>(1.5);
		world->add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

		auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
		world->add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

		auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
		world->add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

		return world;
	}
private:

};

scene::scene()
{
}

scene::~scene()
{
}
