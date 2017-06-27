#include "Heightmap.h"



Heightmap::Heightmap()
{
	srand(time(NULL));
}

void Heightmap::create(int s)
{
	SIZE = s;
	HALF_SIZE = s / 2;

	HEIGHT_MAP_TOP = new float[SIZE*SIZE];
	HEIGHT_MAP_BOT = new float[SIZE*SIZE];
	HEIGHT_MAP_LEFT = new float[SIZE*SIZE];
	HEIGHT_MAP_RIGHT = new float[SIZE*SIZE];
	HEIGHT_MAP_FRONT = new float[SIZE*SIZE];
	HEIGHT_MAP_BACK = new float[SIZE*SIZE];

	createSide(HEIGHT_MAP_TOP);
	createSide(HEIGHT_MAP_BOT);

	createSide(HEIGHT_MAP_LEFT);
	createSide(HEIGHT_MAP_RIGHT);

	createSide(HEIGHT_MAP_FRONT);
	createSide(HEIGHT_MAP_BACK);


	std::cout << "Created Heightmap successfully!" << std::endl;
}

float Heightmap::get(int x, int y, int z)
{
	if (x == HALF_SIZE)
	{
		y += HALF_SIZE;
		z += HALF_SIZE;
		return HEIGHT_MAP_RIGHT[y*SIZE + z];
	}
	else if (x == -HALF_SIZE)
	{
		y += HALF_SIZE;
		z += HALF_SIZE;
		return HEIGHT_MAP_LEFT[y*SIZE + z];
	}
	else if (y == HALF_SIZE)
	{
		x += HALF_SIZE;
		z += HALF_SIZE;
		return HEIGHT_MAP_TOP[x*SIZE + z];
	}
	else if (y == -HALF_SIZE)
	{
		x += HALF_SIZE;
		z += HALF_SIZE;
		return HEIGHT_MAP_BOT[x*SIZE + z];
	}
	else if (z == HALF_SIZE)
	{
		x += HALF_SIZE;
		y += HALF_SIZE;
		return HEIGHT_MAP_FRONT[x*SIZE + y];
	}
	else if (z == -HALF_SIZE)
	{
		x += HALF_SIZE;
		y += HALF_SIZE;
		return HEIGHT_MAP_BACK[x*SIZE + y];
	}
	
}

float Heightmap::get(glm::vec3 p)
{
	
	return get(p.x, p.y, p.z);
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


	HEIGHT_MAP_TOP = new float[SIZE*SIZE];
	HEIGHT_MAP_BOT = new float[SIZE*SIZE];
	HEIGHT_MAP_LEFT = new float[SIZE*SIZE];
	HEIGHT_MAP_RIGHT = new float[SIZE*SIZE];
	HEIGHT_MAP_FRONT = new float[SIZE*SIZE];
	HEIGHT_MAP_BACK = new float[SIZE*SIZE];

	for (int y = 0; y < HEIGHT; y++)
	{
		for (int x = 0; x < WIDTH; x++)
		{
			float r = heightmap[y*WIDTH * 3 + x * 3];
			float g = heightmap[y*WIDTH * 3 + x * 3 + 1];
			float b = heightmap[y*WIDTH * 3 + x * 3 + 2];

			float height = (r + g + b) / 3;
			HEIGHT_MAP_TOP[y*WIDTH + x] = height;
			HEIGHT_MAP_BOT[y*WIDTH + x] = height;
			HEIGHT_MAP_LEFT[y*WIDTH + x] = height;
			HEIGHT_MAP_RIGHT[y*WIDTH + x] = height;
			HEIGHT_MAP_FRONT[y*WIDTH + x] = height;
			HEIGHT_MAP_BACK[y*WIDTH + x] = height;
		}
	}

	std::cout << "Loaded Heightmap successfully!" << std::endl;
	SOIL_free_image_data(heightmap);
}

Heightmap::~Heightmap()
{
	delete[] HEIGHT_MAP_TOP;
	delete[] HEIGHT_MAP_BOT;
	delete[] HEIGHT_MAP_LEFT;
	delete[] HEIGHT_MAP_RIGHT;
	delete[] HEIGHT_MAP_FRONT;
	delete[] HEIGHT_MAP_BACK;
}

//PRIVATE
void Heightmap::createSide(float *side)
{
	for (int i = 0; i < SIZE; i++)
	{
		for (int j = 0; j < SIZE; j++)
		{
			float r = rand() % 1000 + 500;
			float g = rand() % 1000 + 500;
			float b = rand() % 1000 + 500;

			float height = (r + g + b) / 3;
			side[i*SIZE + j] = height;
		}
	}
}