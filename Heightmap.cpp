#include "Heightmap.h"



Heightmap::Heightmap()
{
	srand(time(NULL));
	noise.SetNoiseType(FastNoise::SimplexFractal);
	noise.SetSeed(rand()%10000);
	noise.SetFrequency(0.002);
	noise.SetFractalOctaves(4);
	heightModifier = 1;
}

void Heightmap::create(int s)
{
	SIZE = s;
	HALF_SIZE = s / 2;

	std::cout << "Created Heightmap successfully!" << std::endl;
}

float Heightmap::get(int x, int y)
{
	x = x < 0 ? x + SIZE : x;
	y = y < 0 ? y + SIZE : y;

	x = x%SIZE;
	y = y%SIZE;

	float height = noise.GetNoise(x, y) * heightModifier;

	//height = height * 1.1;

	//height = glm::clamp(height, -200.f, 200.f);

	return height; //HEIGHT_MAP[y*SIZE + x] * heightModifier;
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

	SIZE = w*FEATURE_SIZE;
	HALF_SIZE = SIZE / 2;

	int WIDTH = SIZE;
	int HEIGHT = SIZE;


	HEIGHT_MAP = new float[SIZE*SIZE];

	for (int y = 0, y2 = 0; y < h; y++, y2 += FEATURE_SIZE)
	{
		for (int x = 0, x2 = 0; x < w; x++, x2 += FEATURE_SIZE)
		{
			float r = heightmap[y*w * 3 + x * 3];
			float g = heightmap[y*w * 3 + x * 3 + 1];
			float b = heightmap[y*w * 3 + x * 3 + 2];

			float height = (r + g + b) / 3;
			height -= 128;
			height /= 128;

			set(x2, y2, height);
		}
	}

	std::cout << "Loaded Heightmap successfully!" << std::endl;
	SOIL_free_image_data(heightmap);

	float stepSize = FEATURE_SIZE;
	float scale = 1;

	while (stepSize > 1)
	{
		diamondSquare(stepSize, scale);
		
		stepSize /= 2;
		scale /= 2;
	}
}

void Heightmap::setHeightModifier(float hm)
{
	heightModifier = hm;
}

void Heightmap::set(int x, int y, float v)
{
	x = x < 0 ? x + SIZE : x;
	y = y < 0 ? y + SIZE : y;

	x = x%SIZE;
	y = y%SIZE;

	HEIGHT_MAP[y*SIZE + x] = v;
}

Heightmap::~Heightmap()
{
	delete[] HEIGHT_MAP;
}

//PRIVATE
void Heightmap::squareStep(int x, int y, int size, float value)
{
	int halfSize = size / 2;

	float v1 = get(x - halfSize, y - halfSize) / heightModifier;
	float v2 = get(x + halfSize, y - halfSize) / heightModifier;
	float v3 = get(x - halfSize, y + halfSize) / heightModifier;
	float v4 = get(x + halfSize, y + halfSize) / heightModifier;

	float avg = ((v1 + v2 + v3 + v4) / 4) + value;

	set(x, y, avg);

}

void Heightmap::diamondStep(int x, int y, int size, float value)
{
	int halfSize = size / 2;

	float v1 = get(x - halfSize, y) / heightModifier;
	float v2 = get(x + halfSize, y) / heightModifier;
	float v3 = get(x, y + halfSize) / heightModifier;
	float v4 = get(x, y - halfSize) / heightModifier;

	float avg = ((v1 + v2 + v3 + v4) / 4) + value;

	set(x, y, avg);
}

void Heightmap::diamondSquare(int stepSize, float scale)
{
	float halfStep = stepSize / 2;
	
	for (int y = halfStep; y < SIZE + halfStep; y += stepSize)
	{
		for (int x = halfStep; x < SIZE + halfStep; x += stepSize)
		{
			float random = rand() % 1000;
			random -= 500;
			random /= 500;
			squareStep(x, y, stepSize, random * scale);
		}
	}

	for (int y = 0; y < SIZE; y += stepSize)
	{
		for (int x = 0; x < SIZE; x += stepSize)
		{
			float random = rand() % 1000;
			random -= 500;
			random /= 500;

			diamondStep(x + halfStep, y, stepSize, random * scale);
			diamondStep(x, y + halfStep, stepSize, random * scale);
		}
	}
}