#include "Heightmap.h"



Heightmap::Heightmap()
{
}

void Heightmap::create(int w, int h)
{
	WIDTH = w;
	HEIGHT = h;
	HEIGHT_MAP = new float[WIDTH*HEIGHT];

	for (int y = 0; y < HEIGHT; y++)
	{
		for (int x = 0; x < WIDTH; x++)
		{
			float r = 100;
			float g = 100;
			float b = 100;

			float height = (r + g + b) / 3;
			HEIGHT_MAP[y*WIDTH + x] = height;
		}
	}

	std::cout << "Created Heightmap successfully!" << std::endl;
}

float Heightmap::get(int x, int y)
{
	if (x >= WIDTH)
	{
		x = WIDTH - 1;
	}

	if (y >= HEIGHT)
	{
		y = HEIGHT - 1;
	}

	return HEIGHT_MAP[y * WIDTH + x];
}

int Heightmap::getHeight()
{
	return HEIGHT;
}

int Heightmap::getWidth()
{
	return WIDTH;
}

void Heightmap::load(std::string heightmapFP)
{
	int w, h, channels;
	unsigned char *heightmap = SOIL_load_image(heightmapFP.c_str(), &w, &h, &channels, SOIL_LOAD_RGB);
	if (heightmap == 0)
	{
		std::cout << "SOIL failed to load image." << std::endl;
		return;
	}

	WIDTH = w;
	HEIGHT = h;
	HEIGHT_MAP = new float[WIDTH*HEIGHT];

	for (int y = 0; y < HEIGHT; y++)
	{
		for (int x = 0; x < WIDTH; x++)
		{
			float r = heightmap[y*WIDTH * 3 + x * 3];
			float g = heightmap[y*WIDTH * 3 + x * 3 + 1];
			float b = heightmap[y*WIDTH * 3 + x * 3 + 2];

			float height = (r + g + b) / 3;
			HEIGHT_MAP[y*WIDTH + x] = height;
		}
	}

	std::cout << "Loaded Heightmap successfully!" << std::endl;
	SOIL_free_image_data(heightmap);
}

Heightmap::~Heightmap()
{
	delete[] HEIGHT_MAP;
}
