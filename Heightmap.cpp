#include "Heightmap.h"



Heightmap::Heightmap()
{
	srand(time(NULL));
}

void Heightmap::create(int s)
{
	SIZE = s;
	HALF_SIZE = s / 2;

	HEIGHT_MAP = new float[SIZE*SIZE];

	for (int i = 0; i < SIZE; i++)
	{
		for (int j = 0; j < SIZE; j++)
		{
			float r = rand() % 1000 + 500;
			float g = rand() % 1000 + 500;
			float b = rand() % 1000 + 500;

			float height = (r + g + b) / 3;
			HEIGHT_MAP[i*SIZE + j] = height;
		}
	}

	std::cout << "Created Heightmap successfully!" << std::endl;
}

float Heightmap::get(int x, int y)
{
	x = x%SIZE;
	y = y%SIZE;

	return HEIGHT_MAP[y*SIZE+ x];	
}

float Heightmap::get(glm::vec2 p)
{
	
	return get(p.x, p.y);
}

int Heightmap::getHeight()
{
	return getSize();
}

int Heightmap::getWidth()
{
	return getSize();
}

int Heightmap::getSize()
{
	return SIZE;
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

	if (w != h)
	{
		std::cout << "Image is not square, could not be turned into Heightmap!" << std::endl;
		return;
	}

	SIZE = w;
	HALF_SIZE = w / 2;

	int WIDTH = w;
	int HEIGHT = h;


	HEIGHT_MAP = new float[SIZE*SIZE];

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