class renderer
{
public:
	renderer();
	~renderer();

	void setScene(scene* scene);
	void setPixelBuffer(color* buffer);

	void renderImage();

private:
	void renderLine(int line);

	scene* m_scene;
	camera* m_camera;
	hittableList* m_world;
	color* m_pixelBuffer;

	int m_samplesPerPixel;
	int m_maxBounces;
};

color rayColor(const ray& r, const color& background, const hittable* world, int depth);

renderer::renderer() : m_samplesPerPixel(36), m_maxBounces(8)
{
}

renderer::~renderer()
{
	delete m_camera;
	delete m_world;
}

inline void renderer::setScene(scene* scene)
{
	// Replace scene
	m_scene = scene;

	// Replace camera
	delete m_camera;
	m_camera = scene->getCamera();

	// Replace world
	delete m_world;
	m_world = scene->getWorld();
}

inline void renderer::setPixelBuffer(color* buffer)
{
	m_pixelBuffer = buffer;
}

inline void renderer::renderImage()
{
	std::cout << "  0% done of rendering the image\r";
	for (int y = m_scene->getHeigth()-1; y >= 0; --y)
	{
		double percent = (y + 1.0) / m_scene->getHeigth() * 100;
		int percentInt = 100 - (int) std::round(percent);
		std::cout << std::setw(3) << percentInt << "%\r";
		renderLine(y);
	}
}

inline void renderer::renderLine(int line) {
	for (int x = 0; x < m_scene->getWidth(); ++x)
	{
		color pixel_color(0, 0, 0);
		for (int s = 0; s < m_samplesPerPixel; ++s) {
			auto u = (x + randomDouble()) / (m_scene->getWidth() - 1);
			auto v = (line + randomDouble()) / (m_scene->getHeigth() - 1);
			ray r = m_camera->get_ray(u, v);
			pixel_color += rayColor(r, m_scene->getBackgroundColor(), m_world, m_maxBounces);
		}
		translateColor(pixel_color, m_samplesPerPixel);

		m_pixelBuffer[line * m_scene->getWidth() + x] = pixel_color;
	}
}

color rayColor(const ray& r, const color& background, const hittable* world, int depth) {
	hitRecord rec;

	// If we've exceeded the ray bounce limit, no more light is gathered.
	if (depth <= 0)
		return color(0, 0, 0);

	// If the ray hits nothing, return the background color.
	if (!world->hit(r, 0.001, infinity, rec))
		return background;

	ray scattered;
	color attenuation;
	color emitted = rec.matPtr->emitted(rec.u, rec.v, rec.p);

	if (!rec.matPtr->scatter(r, rec, attenuation, scattered))
		return emitted;

	return emitted + attenuation * rayColor(scattered, background, world, depth - 1);
}