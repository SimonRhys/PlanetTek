#pragma once

#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <time.h>
#include <SOIL/SOIL.h>
#include <glm/glm.hpp>

class Heightmap
{
public:
	Heightmap();
	~Heightmap();

	void create(int size);

	float get(int x, int y);
	float get(glm::vec2 p);
	int getHeight();
	int getWidth();
	int getSize();

	void load(std::string heightmapFP);

private:
	float *HEIGHT_MAP;
	
	int SIZE;
	int HALF_SIZE;
};

