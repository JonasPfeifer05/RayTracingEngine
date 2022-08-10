class renderer
{
public:
	renderer();
	~renderer();

	void setScene(scene* scene);
	void setPixelBuffer(color* buffer);

	void renderImageSingleThreaded();
	void renderImageMultiThreaded();

	void renderLine(int line);

private:
	scene* m_scene;
	camera* m_camera;
	hittableList* m_world;
	color* m_pixelBuffer;

	int m_samplesPerPixel;
	int m_maxBounces;

	double m_threadScale;
};

color rayColor(const ray& r, const color& background, const hittable* world, int depth);

renderer::renderer() : m_samplesPerPixel(10), m_maxBounces(50), m_threadScale(1)
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

void renderThreaded(renderer* renderer, std::mutex& coutMutex, std::atomic<int>& nextJob, std::atomic<int>& jobsDone, int height)
{
	while (nextJob >= 0) {
		int line = nextJob--;

		renderer->renderLine(line);

		++jobsDone;


		double percent = (line + 1.0) / height * 100;
		int percentInt = 100 - (int)std::round(percent);
		{
			std::lock_guard<std::mutex> lock(coutMutex);
			std::cout << std::setw(3) << percentInt << "%\r";
		}
	}
}

inline void renderer::renderImageMultiThreaded()
{
	std::mutex coutMutex;
	std::atomic<int> nextJob{ m_scene->getHeigth() - 1 };
	std::atomic<int> jobsDone{ 0 };

	int threadNum = std::thread::hardware_concurrency() * m_threadScale;

	std::vector<std::thread> workers;

	int i = 0;
	while (i < threadNum && i < m_scene->getHeigth()) {
		workers.push_back(std::thread(renderThreaded, this, std::ref(coutMutex), std::ref(nextJob), std::ref(jobsDone), m_scene->getHeigth()));

		i++;
	}
	threadNum = i;
	{
		std::lock_guard<std::mutex> lock(coutMutex);
		std::cout << "Launched " << threadNum << " threads to render the image" << std::endl;
		std::cout << "  0% done of rendering the image\r";
	}


	for (auto& worker : workers) {
		worker.join();
	}
}

inline void renderer::renderImageSingleThreaded()
{
	std::cout << "  0% done of rendering the image\r";
	for (int y = m_scene->getHeigth() - 1; y >= 0; --y)
	{
		double percent = (y + 1.0) / m_scene->getHeigth() * 100;
		int percentInt = 100 - (int)std::round(percent);
		std::cout << std::setw(3) << percentInt << "%\r";
		renderLine(y);
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