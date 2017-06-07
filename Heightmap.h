#pragma once

#include <iostream>
#include <string>
#include <SOIL/SOIL.h>

class Heightmap
{
public:
	Heightmap();
	~Heightmap();

	void create(int w, int h);

	float get(int x, int y);
	int getHeight();
	int getWidth();

	void load(std::string heightmapFP);

private:
	float *HEIGHT_MAP;
	int WIDTH;
	int HEIGHT;
};

