#include "scenes/scene.h"
#include <string>
#include "renderer.h"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <chrono>

class application
{
public:
	application();
	~application();

	void load(scene* _scene);
	void start(bool multiThreaded);
	void output(std::string outName);

private:
	renderer m_renderer;
	color* m_pixelBuffer;
	int m_bufferLength;
	scene* m_scene;
};

application::application()
{
	m_pixelBuffer = new color[0];
	m_bufferLength = 0;
	m_scene = new scene();
}

application::~application()
{
	delete[] m_pixelBuffer;
	delete m_scene;
}

inline void application::load(scene* _scene)
{
	std::cout << "loading scene..." << std::endl;
	// Replace old scene with new scene
	delete m_scene;
	m_scene = _scene;

	// Setup Pixel buffer //
	// Get new length
	m_bufferLength = _scene->getLength();

	// Replace old pixelbuffer and replace with new one
	delete m_pixelBuffer;
	m_pixelBuffer = new color[m_bufferLength];

	m_renderer.setScene(m_scene);
	m_renderer.setPixelBuffer(m_pixelBuffer);
}

inline void application::start(bool multiThreaded)
{
	auto start = std::chrono::high_resolution_clock::now();

	std::cout << "\nStarting to render image\n";
	if (multiThreaded)
	{
		m_renderer.renderImageMultiThreaded();
	}
	else {
		m_renderer.renderImageSingleThreaded();
	}
	std::cout << "\nFinished rendering the image\n";

	auto stop = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
	std::cout << "Rendering took " << duration.count() / 1000000 << " seconds" << std::endl;
}

inline void application::output(std::string outName)
{
	std::string outNamePath = "./RaytracingEngine/output/" + outName;

	std::ofstream file = std::ofstream(outNamePath);
	file.clear();

	std::cout << "\nStarting to write to file with name: \"" << outName << "\"\n\r";
	std::cout << "  0% done of writing to file with name \"" << outName << "\"\r";

	file << "P3\n" << m_scene->getWidth() << " " << m_scene->getHeigth() << "\n" << "255" << "\n";

	for (int i = m_scene->getHeigth()-1; i >= 0; i--)
	{
		for (int k = 0; k < m_scene->getWidth(); k++) {
			writeColorStream(file, m_pixelBuffer[i*m_scene->getWidth() + k]);
		}

		double percent = (i + 1) * 1.0 / m_scene->getHeigth();
		percent *= 100;
		int percentInt = 100 - (int)std::round(percent);
		std::cout << std::setw(3) << percentInt << "%" << "\r";
	}

	std::cout << "\nFinished writing image to file with name \"" << outName << "\"\n";

	file.close();
}
