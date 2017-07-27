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
	void setHeightModifier(float hm);

	void set(int x, int y, float v);

private:
	void squareStep(int x, int y, int size, float value);
	void diamondStep(int x, int y, int size, float value);
	void diamondSquare(int stepsize, float scale);



	float *HEIGHT_MAP;
	float heightModifier;
	
	int SIZE;
	int HALF_SIZE;
	int FEATURE_SIZE = 4;
};

