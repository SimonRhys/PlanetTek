#include "Planet.h"


//Public
Planet::Planet(float radius)
{
	this->radius = radius;
	this->reloading = false;
	heightmap.create(64);
	this->createShaderProgram();
	this->generate();
}

Planet::Planet(float radius, std::string heightmapFP)
{
	this->radius = radius;
	this->reloading = false;
	heightmap.load(heightmapFP);
	this->createShaderProgram();
	this->generate();	
}

void Planet::draw(glm::mat4 proj, glm::mat4 view) 
{
	for (int i = 0; i < MAX_TERRAIN_BLOCKS_TOTAL; i++)
	{
		if (texture == 0)
		{
			terrainBlocks[i].draw(proj, view, radius);
		}
		else
		{
			terrainBlocks[i].draw(proj, view, radius, texture);
		}
	}
}

void Planet::loadTextures(std::string filePath)
{
	int w, h, channels;
	unsigned char *image = SOIL_load_image(filePath.c_str(), &w, &h, &channels, SOIL_LOAD_RGB);
	if (image == 0)
	{
		std::cout << "SOIL failed to load image." << std::endl;
		return;
	}
	else
	{
		std::cout << "SOIL loaded Planet textures!" << std::endl;
	}

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);


	SOIL_free_image_data(image);
}

void Planet::setPlayerCamera(glm::vec3 *playerCamera)
{
	this->playerCamera = playerCamera;
}

void Planet::update(float dt)
{
	if (this->reloading)
	{
		return; 
	}

	int mapCounter = 0;

	for (int i = -3; i <= 3; i++)
	{
		for (int j = -3; j <= 3; j++)
		{
			glm::vec2 startPoint = lodMap[mapCounter]->getStartPoint();
			glm::vec2 endPoint = lodMap[mapCounter]->getEndPoint();

			if (intersect(startPoint, endPoint, glm::vec2(playerCamera->x, playerCamera->y)) &&
				(i != 0 || j != 0))
			{
				//RELOAD LOD (IGNORE CORNER CASES FOR NOW)
				if (i == 1 && j == 0)
				{
					for (int k = 0; k < lodMap.size()-7; k++)
					{
						lodMap[k] = lodMap[k+7];
					}

					//For the last 7 chunks we need to create the far LOD
					for (int k = lodMap.size()-7; k < lodMap.size(); k++)
					{
						glm::vec2 modifier(0, CHUNK_SIZE.y);
						genMap.push_back(std::pair<int, glm::vec2>(k, modifier));
					}

					//BELOW NEEDS TO CHANGE TOO
					regenMap.push_back(std::pair<int, int>(16, 5));
					regenMap.push_back(std::pair<int, int>(17, 5));
					regenMap.push_back(std::pair<int, int>(18, 5));

					regenMap.push_back(std::pair<int, int>(37, 1));
					regenMap.push_back(std::pair<int, int>(38, 1));
					regenMap.push_back(std::pair<int, int>(39, 1));

					regenMap.push_back(std::pair<int, int>(44, 5));
					regenMap.push_back(std::pair<int, int>(45, 5));
					regenMap.push_back(std::pair<int, int>(46, 5));
				}
				else if (i == -1 && j == 0)
				{
					for (int k = lodMap.size(); k > 6; k++)
					{
						lodMap[k] = lodMap[k - 7];
					}

					//For k = [0, 6] we need to create the far LOD
					for (int k = 0; k <= 6; k++)
					{
						glm::vec2 newStart = lodMap[k]->getStartPoint();
						glm::vec2 newEnd = lodMap[k]->getEndPoint();

						newStart.y += CHUNK_SIZE.y;
						newEnd.y += CHUNK_SIZE.y;

					}

					regenMap.push_back(std::pair<int, int>(2, 5));
					regenMap.push_back(std::pair<int, int>(3, 5));
					regenMap.push_back(std::pair<int, int>(4, 5));

					regenMap.push_back(std::pair<int, int>(9, 1));
					regenMap.push_back(std::pair<int, int>(10, 1));
					regenMap.push_back(std::pair<int, int>(11, 1));

					regenMap.push_back(std::pair<int, int>(30, 5));
					regenMap.push_back(std::pair<int, int>(31, 5));
					regenMap.push_back(std::pair<int, int>(32, 5));
				}
				else if (i == 0 && j == -1)
				{
					regenMap.push_back(std::pair<int, int>(25, 5));
					regenMap.push_back(std::pair<int, int>(18, 5));
					regenMap.push_back(std::pair<int, int>(32, 5));

					regenMap.push_back(std::pair<int, int>(22, 1));
					regenMap.push_back(std::pair<int, int>(15, 1));
					regenMap.push_back(std::pair<int, int>(29, 1));

					regenMap.push_back(std::pair<int, int>(21, 5));
					regenMap.push_back(std::pair<int, int>(14, 5));
					regenMap.push_back(std::pair<int, int>(28, 5));
				}
				else if (i == 0 && j == 1)
				{
					regenMap.push_back(std::pair<int, int>(23, 5));
					regenMap.push_back(std::pair<int, int>(16, 5));
					regenMap.push_back(std::pair<int, int>(30, 5));

					regenMap.push_back(std::pair<int, int>(26, 1));
					regenMap.push_back(std::pair<int, int>(19, 1));
					regenMap.push_back(std::pair<int, int>(33, 1));

					regenMap.push_back(std::pair<int, int>(27, 5));
					regenMap.push_back(std::pair<int, int>(17, 5));
					regenMap.push_back(std::pair<int, int>(34, 5));
				}

				this->reloadLODs();

				//STOP LOOPING
				//return
			}

			mapCounter++;
		}
	}


}

Planet::~Planet()
{
	glDeleteTextures(1, &texture);
}

//Private
void Planet::createShaderProgram()
{
	shader.createShader("planetVert.sh", GL_VERTEX_SHADER);
	shader.createShader("planetFrag.sh", GL_FRAGMENT_SHADER);
	shader.createProgram();

	uniformLocations["projection"] = glGetUniformLocation(shader.getShaderProgram(), "projection");
	uniformLocations["view"] = glGetUniformLocation(shader.getShaderProgram(), "view");
	uniformLocations["model"] = glGetUniformLocation(shader.getShaderProgram(), "model");
	uniformLocations["lightPos"] = glGetUniformLocation(shader.getShaderProgram(), "lightPos");
	uniformLocations["lightColor"] = glGetUniformLocation(shader.getShaderProgram(), "lightColor");

	for (int i = 0; i < MAX_TERRAIN_BLOCKS_TOTAL; i++)
	{
		terrainBlocks[i].create(&shader, &uniformLocations);
	}
}

void Planet::generate()
{
	glm::vec2 playerPos(30, 30);
	glm::vec2 centre = playerPos;

	int blockCount = 0;

	for (int i = -3; i <= 3; i++)
	{
		for (int j = -3; j <= 3; j++)
		{
			glm::vec2 start = centre;
			glm::vec2 end;


			start.x = centre.x + j*CHUNK_SIZE.x;
			start.y = centre.y + i*CHUNK_SIZE.y;

			end = start;
			end.x = start.x + CHUNK_SIZE.x;
			end.y = start.y + CHUNK_SIZE.y;

			glm::vec2 distanceVector = glm::abs(glm::vec2(j, i));
			int distanceFromCentre = glm::max(distanceVector.x, distanceVector.y);
			int lod;

			if (distanceFromCentre == 3)
			{
				lod = 10;
			}
			else if (distanceFromCentre == 2)
			{
				lod = 5;
			}
			else 
			{
				lod = 1;
			}

			terrainBlocks[blockCount].generate(start, end, &heightmap, radius, lod);
			lodMap.push_back(&terrainBlocks[blockCount]);
			blockCount++;
		}
	}

	/*int blockCount = 0;

	terrainBlocks[blockCount].generate(glm::vec2(0, 0), glm::vec2(64, 64), &heightmap, radius, 1);
	lodMap.push_back(&terrainBlocks[blockCount]);
	blockCount++;*/
}

void Planet::reloadLODs()
{
	std::cout << "Reloading LODs..." << std::endl;

	this->reloading = true;

	for (int i=0; i < regenMap.size(); i++)
	{
		TerrainBlock* currBlock = lodMap[regenMap[i].first];

		for (int j = 0; j < MAX_TERRAIN_BLOCKS_TOTAL; j++)
		{
			if (!terrainBlocks[j].isUsed())
			{
				glm::vec2 startPoint = currBlock->getStartPoint();
				glm::vec2 endPoint = currBlock->getEndPoint();

				terrainBlocks[j].generate(startPoint, endPoint, &heightmap, radius, regenMap[i].second);
				currBlock->markUnused();
				
				lodMap[regenMap[i].first] = &terrainBlocks[j];

				break;
			}
		}
	}

	regenMap.clear();

	//this->reloading = false;

}

bool Planet::intersect(glm::vec2 startPoint, glm::vec2 endPoint, glm::vec2 v)
{
	glm::vec3 s;
	glm::vec3 e;

	s.x = glm::min(startPoint.x, endPoint.x);
	s.y = glm::min(startPoint.y, endPoint.y);

	e.x = glm::max(startPoint.x, endPoint.x);
	e.y = glm::max(startPoint.y, endPoint.y);

	if (v.x >= s.x && v.x <= e.x &&
		v.y >= s.y && v.y <= e.y)
	{
		return true;
	}

	return false;
}

bool Planet::intersect(std::vector<glm::vec2> startList, std::vector<glm::vec2> endList, glm::vec2 v)
{
	if (startList.size() != endList.size())
	{
		std::cout << "Intersection cannot be found as there were differing numbers of starting points and end points!" << std::endl;
		return false;
	}

	for (int i = 0; i < startList.size(); i++)
	{
		if (intersect(startList[i], endList[i], v))
		{
			return true;
		}
	}

	return false;


}

bool inCubeBoundry(glm::vec3 p, int len)
{
	bool inBoundry = true;

	inBoundry = inBoundry && glm::abs(p.x) < len;
	inBoundry = inBoundry && glm::abs(p.y) < len;
	inBoundry = inBoundry && glm::abs(p.z) < len;

	return inBoundry;


	int width = 0;
	int height = 0;
	int radius = 10;
	glm::vec3 pointsOnPlane;

	float thetaDelta = PI / width;
	float phiDelta = TWO_PI / height;
	float theta = pointsOnPlane.x * thetaDelta;
	float phi = pointsOnPlane.y * phiDelta;

	glm::vec3 pointsOnSphere;
	pointsOnSphere.x = radius * glm::sin(theta) * glm::cos(phi);
	pointsOnSphere.y = radius * glm::sin(theta) * glm::sin(phi);
	pointsOnSphere.z = radius * glm::cos(theta);
}
