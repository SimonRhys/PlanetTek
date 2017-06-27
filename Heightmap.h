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

	float get(int x, int y, int z);
	float get(glm::vec3 p);
	int getHeight();
	int getWidth();
	int getSize();

	void load(std::string heightmapFP);

private:
	void createSide(float *side);

	float *HEIGHT_MAP_TOP;
	float *HEIGHT_MAP_BOT;
	float *HEIGHT_MAP_LEFT;
	float *HEIGHT_MAP_RIGHT;
	float *HEIGHT_MAP_FRONT;
	float *HEIGHT_MAP_BACK;

	
	int SIZE;
	int HALF_SIZE;
};

