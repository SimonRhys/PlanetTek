#include "Planet.h"


//Public
Planet::Planet(float radius)
{
	this->radius = radius;
	heightmap.create(64);
	this->createShaderProgram();
	this->generate();
}

Planet::Planet(float radius, std::string heightmapFP)
{
	this->radius = radius;
	heightmap.load(heightmapFP);
	this->createShaderProgram();
	this->generate();
	
}

void Planet::draw(glm::mat4 proj, glm::mat4 view) 
{
	for (int i = 0; i < MAX_TERRAIN_BLOCKS_HQ; i++)
	{
		terrainBlocksHQ[i].draw(proj, view, radius, 1);
	}
	for (int i = 0; i < MAX_TERRAIN_BLOCKS_MQ; i++)
	{
		terrainBlocksMQ[i].draw(proj, view, radius, 5);
	}
	for (int i = 0; i < MAX_TERRAIN_BLOCKS_LQ; i++)
	{
		terrainBlocksLQ[i].draw(proj, view, radius, 10);
	}

}

void Planet::setPlayerCamera(glm::vec3 *playerCamera)
{
	this->playerCamera = playerCamera;
}

void Planet::update(float dt)
{
	glm::vec3 newPlayerPos(6, 6, 32);

	int mapCounter = 0;

	for (int i = -3; i < 3; i++)
	{
		for (int j = -3; j < 3; j++)
		{
			std::vector<glm::vec3> startList = lodMap[mapCounter]->getStartPoints();
			std::vector<glm::vec3> endList = lodMap[mapCounter]->getEndPoints();

			if (intersect(startList, endList, newPlayerPos) && (i != 0 || j != 0))
			{
				//RELOAD LOD (IGNORE CORNER CASES FOR NOW)
				if (i == -1 && j == 0)
				{
					regenMap.insert_or_assign(16, 5);
					regenMap.insert_or_assign(17, 5);
					regenMap.insert_or_assign(18, 5);

					regenMap.insert_or_assign(37, 1);
					regenMap.insert_or_assign(38, 1);
					regenMap.insert_or_assign(39, 1);

					regenMap.insert_or_assign(44, 5);
					regenMap.insert_or_assign(45, 5);
					regenMap.insert_or_assign(46, 5);
				}
				else if (i == 1 && j == 0)
				{
					regenMap.insert_or_assign(2, 5);
					regenMap.insert_or_assign(3, 5);
					regenMap.insert_or_assign(4, 5);

					regenMap.insert_or_assign(9, 1);
					regenMap.insert_or_assign(10, 1);
					regenMap.insert_or_assign(11, 1);

					regenMap.insert_or_assign(30, 5);
					regenMap.insert_or_assign(31, 5);
					regenMap.insert_or_assign(32, 5);
				}
				else if (i == 0 && j == -1)
				{

				}
				else if (i == 0 && j == 1)
				{

				}
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

	for (int i = 0; i < MAX_TERRAIN_BLOCKS_HQ; i++)
	{
		terrainBlocksHQ[i].create(&shader, &uniformLocations);
	}
	for (int i = 0; i < MAX_TERRAIN_BLOCKS_MQ; i++)
	{
		terrainBlocksMQ[i].create(&shader, &uniformLocations);
	}
	for (int i = 0; i < MAX_TERRAIN_BLOCKS_LQ; i++)
	{
		terrainBlocksLQ[i].create(&shader, &uniformLocations);
	}
}

void Planet::generate()
{
	glm::vec3 playerPos(0, 0, 32);
	glm::vec3 modifier(1, 1, 0);


	glm::vec3 centre = playerPos;

	int blockCountHQ = 0;
	int blockCountMQ = 0;
	int blockCountLQ = 0;

	int mapCounter = 0;



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
				start.y = centre.y + i*modifier.y*CHUNK_SIZE.x;
				start.z = centre.z + j*modifier.z*CHUNK_SIZE.y;

				end = start;
				end.y = start.y + CHUNK_SIZE.x;
				end.z = start.z + CHUNK_SIZE.y;
			}

			if (modifier.y == 0)
			{
				start.x = centre.x + i*modifier.x*CHUNK_SIZE.x;
				start.z = centre.z + j*modifier.z*CHUNK_SIZE.y;

				end = start;
				end.x = start.x + CHUNK_SIZE.x;
				end.z = start.z + CHUNK_SIZE.y;
			}

			if (modifier.z == 0)
			{
				start.x = centre.x + i*modifier.x*CHUNK_SIZE.x;
				start.y = centre.y + j*modifier.y*CHUNK_SIZE.y;

				end = start;
				end.x = start.x + CHUNK_SIZE.x;
				end.y = start.y + CHUNK_SIZE.y;
			}

			startList.push_back(start);
			endList.push_back(end);

			ensureLimits(&startList, &endList, 'x');
			ensureLimits(&startList, &endList, 'y');
			ensureLimits(&startList, &endList, 'z');

			glm::vec2 distanceVector = glm::abs(glm::vec2(i, j));
			int distanceFromCentre = glm::max(distanceVector.x, distanceVector.y);

			if (distanceFromCentre == 3)
			{
				if (startList.size() > 0)
				{
					terrainBlocksLQ[blockCountLQ].generate(startList, endList, &heightmap, radius, 10);
				}
				else
				{
					terrainBlocksLQ[blockCountLQ].generate(start, end, &heightmap, radius, 10);
				}

				lodMap.insert_or_assign(mapCounter++, &terrainBlocksLQ[blockCountLQ]);

				blockCountLQ++;
			}
			else if (distanceFromCentre == 2)
			{
				if (startList.size() > 0)
				{
					terrainBlocksMQ[blockCountMQ].generate(startList, endList, &heightmap, radius, 5);
				}
				else
				{
					terrainBlocksMQ[blockCountMQ].generate(start, end, &heightmap, radius, 5);
				}

				lodMap.insert_or_assign(mapCounter++, &terrainBlocksMQ[blockCountMQ]);

				blockCountMQ++;
			}
			else 
			{
				if (startList.size() > 0)
				{
					terrainBlocksHQ[blockCountHQ].generate(startList, endList, &heightmap, radius, 1);
				}
				else
				{
					terrainBlocksHQ[blockCountHQ].generate(start, end, &heightmap, radius, 1);
				}

				lodMap.insert_or_assign(mapCounter++, &terrainBlocksHQ[blockCountHQ]);

				blockCountHQ++;
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

		if (*startPointer > glm::abs(heightmap.getWidth() / 2))
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

			float overshoot = glm::abs(*startPointer - heightmap.getWidth() / 2);
			start->x = start->x + overshoot*newModifier.x;
			start->y = start->y + overshoot*newModifier.y;
			start->z = start->z + overshoot*newModifier.z;

			*startPointer = (heightmap.getWidth() / 2)*glm::sign(*startPointer);
			startChanged = true;
		}


		if (*endPointer > glm::abs(heightmap.getWidth() / 2))
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

			float overshoot = glm::abs(*endPointer - heightmap.getWidth() / 2);
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
