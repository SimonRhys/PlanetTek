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
		terrainBlocks[i].draw(proj, view, radius);
	}
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
			std::vector<glm::vec3> startList = lodMap[mapCounter]->getStartPoints();
			std::vector<glm::vec3> endList = lodMap[mapCounter]->getEndPoints();

			if (intersect(startList, endList, *playerCamera) && (i != 0 || j != 0))
			{
				//RELOAD LOD (IGNORE CORNER CASES FOR NOW)
				if (i == -1 && j == 0)
				{
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
				else if (i == 1 && j == 0)
				{
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
	uniformLocations["objectColor"] = glGetUniformLocation(shader.getShaderProgram(), "objectColor");

	for (int i = 0; i < MAX_TERRAIN_BLOCKS_TOTAL; i++)
	{
		terrainBlocks[i].create(&shader, &uniformLocations);
	}
}

void Planet::generate()
{
	glm::vec3 playerPos(0, 0, 32);
	glm::vec3 modifier(1, 1, 0);


	glm::vec3 centre = playerPos;

	int blockCount = 0;

	for (int i = -3; i <= 3; i++)
	{
		for (int j = -3; j <= 3; j++)
		{
			std::vector<glm::vec3> startList;
			std::vector<glm::vec3> endList;

			glm::vec3 start = centre;
			glm::vec3 end;

			if (modifier.x == 0)
			{
				start.y = centre.y + j*modifier.y*CHUNK_SIZE.x;
				start.z = centre.z + i*modifier.z*CHUNK_SIZE.y;

				end = start;
				end.y = start.y + CHUNK_SIZE.x;
				end.z = start.z + CHUNK_SIZE.y;
			}

			if (modifier.y == 0)
			{
				start.x = centre.x + j*modifier.x*CHUNK_SIZE.x;
				start.z = centre.z + i*modifier.z*CHUNK_SIZE.y;

				end = start;
				end.x = start.x + CHUNK_SIZE.x;
				end.z = start.z + CHUNK_SIZE.y;
			}

			if (modifier.z == 0)
			{
				start.x = centre.x + j*modifier.x*CHUNK_SIZE.x;
				start.y = centre.y + i*modifier.y*CHUNK_SIZE.y;

				end = start;
				end.x = start.x + CHUNK_SIZE.x;
				end.y = start.y + CHUNK_SIZE.y;
			}

			startList.push_back(start);
			endList.push_back(end);

			ensureLimits(&startList, &endList, 'x');
			ensureLimits(&startList, &endList, 'y');
			ensureLimits(&startList, &endList, 'z');

			glm::vec2 distanceVector = glm::abs(glm::vec2(j, i));
			int distanceFromCentre = glm::max(distanceVector.x, distanceVector.y);

			if (distanceFromCentre == 3)
			{
				if (startList.size() > 0)
				{
					terrainBlocks[blockCount].generate(startList, endList, &heightmap, radius, 10);
				}
				else
				{
					terrainBlocks[blockCount].generate(start, end, &heightmap, radius, 10);
				}

				lodMap.push_back(&terrainBlocks[blockCount]);

				blockCount++;
			}
			else if (distanceFromCentre == 2)
			{
				if (startList.size() > 0)
				{
					terrainBlocks[blockCount].generate(startList, endList, &heightmap, radius, 5);
				}
				else
				{
					terrainBlocks[blockCount].generate(start, end, &heightmap, radius, 5);
				}

				lodMap.push_back(&terrainBlocks[blockCount]);

				blockCount++;
			}
			else 
			{
				if (startList.size() > 0)
				{
					terrainBlocks[blockCount].generate(startList, endList, &heightmap, radius, 1);
				}
				else
				{
					terrainBlocks[blockCount].generate(start, end, &heightmap, radius, 1);
				}

				lodMap.push_back( &terrainBlocks[blockCount]);

				blockCount++;
			}
		}
	}
}

void Planet::ensureLimits(std::vector<glm::vec3> *startList, std::vector<glm::vec3> *endList, char toCheck)
{
	std::vector<glm::vec3> startToAdd;
	std::vector<glm::vec3> endToAdd;

	int len = startList->size();
	for (int i = 0; i < len; i++)
	{
		glm::vec3 modifier = endList->at(i) - startList->at(i);
		modifier = glm::sign(modifier);

		float *startPointer;
		float *endPointer;
		if (toCheck == 'x')
		{
			startPointer = &startList->at(i).x;
			endPointer = &endList->at(i).x;
		}
		else if (toCheck == 'y')
		{
			startPointer = &startList->at(i).y;
			endPointer = &endList->at(i).y;
		}
		else if (toCheck == 'z')
		{
			startPointer = &startList->at(i).z;
			endPointer = &endList->at(i).z;
		}
		else
		{
			return;
		}

		glm::vec3 *start = &startList->at(i);
		glm::vec3 *end = &endList->at(i);

		bool startChanged = false;

		if (glm::abs(*startPointer) > heightmap.getWidth() / 2)
		{
			glm::vec3 newModifier(0, 0, 0);

			if (modifier.x == 0)
			{
				newModifier.x = -glm::sign(start->x);
			}
			else if (modifier.y == 0)
			{
				newModifier.y = -glm::sign(start->y);
			}
			else if (modifier.z == 0)
			{
				newModifier.z = -glm::sign(start->z);
			}

			float overshoot = glm::abs(*startPointer) - heightmap.getWidth() / 2;
			start->x = start->x + overshoot*newModifier.x;
			start->y = start->y + overshoot*newModifier.y;
			start->z = start->z + overshoot*newModifier.z;

			*startPointer = (heightmap.getWidth() / 2)*glm::sign(*startPointer);
			startChanged = true;
		}


		if (glm::abs(*endPointer) > heightmap.getWidth() / 2)
		{
			glm::vec3 newModifier(0, 0, 0);

			if (modifier.x == 0)
			{
				newModifier.x = -glm::sign(start->x);

				if (!startChanged)
				{
					startToAdd.push_back(*start);

					if (&end->y == endPointer)
					{
						endToAdd.push_back(glm::vec3(heightmap.getWidth() / 2, heightmap.getWidth() / 2, end->z));
					}
					else if (&end->z == endPointer)
					{
						endToAdd.push_back(glm::vec3(heightmap.getWidth() / 2, end->y, heightmap.getWidth() / 2));
					}

					*startPointer = (heightmap.getWidth() / 2)*glm::sign(*endPointer);
					start->x = (heightmap.getWidth() / 2)*glm::sign(end->x);
				}
			}
			else if (modifier.y == 0)
			{
				newModifier.y = -glm::sign(end->y);


				if (!startChanged)
				{
					startToAdd.push_back(*start);

					if (&end->x == endPointer)
					{
						endToAdd.push_back(glm::vec3(heightmap.getWidth() / 2, heightmap.getWidth() / 2, end->z));
					}
					else if (&end->z == endPointer)
					{
						endToAdd.push_back(glm::vec3(end->x, heightmap.getWidth() / 2, heightmap.getWidth() / 2));
					}

					*startPointer = (heightmap.getWidth() / 2)*glm::sign(*endPointer);
					start->y = (heightmap.getWidth() / 2)*glm::sign(end->y);
				}
			}
			else if (modifier.z == 0)
			{
				newModifier.z = -glm::sign(end->z);

				if (!startChanged)
				{
					startToAdd.push_back(*start);

					if (&end->x == endPointer)
					{
						endToAdd.push_back(glm::vec3(heightmap.getWidth() / 2, end->y, heightmap.getWidth() / 2));
					}
					else if (&end->y == endPointer)
					{
						endToAdd.push_back(glm::vec3(end->x, heightmap.getWidth() / 2, heightmap.getWidth() / 2));
					}

					*startPointer = (heightmap.getWidth() / 2)*glm::sign(*endPointer);
					start->z = (heightmap.getWidth() / 2)*glm::sign(end->z);
				}
			}

			float overshoot = glm::abs(*endPointer) - heightmap.getWidth() / 2;
			end->x = end->x + overshoot*newModifier.x;
			end->y = end->y + overshoot*newModifier.y;
			end->z = end->z + overshoot*newModifier.z;

			*endPointer = (heightmap.getWidth() / 2)*glm::sign(*endPointer);

			startToAdd.push_back(*start);
			endToAdd.push_back(*end);
		
		}
	}

	if (startToAdd.size() > 0)
	{
		startList->clear();
		endList->clear();
		for (int i = 0; i < startToAdd.size(); i++)
		{
			startList->push_back(startToAdd[i]);
			endList->push_back(endToAdd[i]);
		}
	}

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
				std::vector<glm::vec3> startPoints = currBlock->getStartPoints();
				std::vector<glm::vec3> endPoints = currBlock->getEndPoints();

				terrainBlocks[j].generate(startPoints, endPoints, &heightmap, radius, regenMap[i].second);
				currBlock->markUnused();
				
				lodMap[regenMap[i].first] = &terrainBlocks[j];

				break;
			}
		}
	}

	regenMap.clear();

	//this->reloading = false;

}

bool Planet::intersect(std::vector<glm::vec3> startList, std::vector<glm::vec3> endList, glm::vec3 v)
{
	if (startList.size() != endList.size())
	{
		std::cout << "Intersection cannot be found as there were differing numbers of starting points and end points!" << std::endl;
		return false;
	}

	for (int i = 0; i < startList.size(); i++)
	{
		glm::vec3 s;
		glm::vec3 e;

		s.x = glm::min(startList[i].x, endList[i].x);
		s.y = glm::min(startList[i].y, endList[i].y);
		s.z = glm::min(startList[i].z, endList[i].z);

		e.x = glm::max(startList[i].x, endList[i].x);
		e.y = glm::max(startList[i].y, endList[i].y);
		e.z = glm::max(startList[i].z, endList[i].z);

		if (v.x >= s.x && v.x <= e.x &&
			v.y >= s.y && v.y <= e.y &&
			v.z >= s.z && v.z <= e.z)
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
