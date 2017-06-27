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
	glm::vec3 playerPos(20, 20, 32);
	glm::vec3 modifier(1, 1, 0);


	glm::vec3 centre = playerPos;

	int blockCountHQ = 0;
	int blockCountMQ = 0;
	int blockCountLQ = 0;



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
