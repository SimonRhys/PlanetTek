#include "Planet.h"


//Public
Planet::Planet(float radius=100)
{
	this->radius = radius;

	shader.createShader("planetVert.sh", GL_VERTEX_SHADER);
	shader.createShader("planetFrag.sh", GL_FRAGMENT_SHADER);
	shader.createProgram();

	uniformLocations["projection"] = glGetUniformLocation(shader.getShaderProgram(), "projection");
	uniformLocations["view"] = glGetUniformLocation(shader.getShaderProgram(), "view");
	uniformLocations["model"] = glGetUniformLocation(shader.getShaderProgram(), "model");
	uniformLocations["lightPos"] = glGetUniformLocation(shader.getShaderProgram(), "lightPos");
	uniformLocations["lightColor"] = glGetUniformLocation(shader.getShaderProgram(), "lightColor");
	uniformLocations["objectColor"] = glGetUniformLocation(shader.getShaderProgram(), "objectColor");

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
void Planet::generate() 
{
	int blockCountHQ = 0;
	int blockCountMQ = 0;
	int blockCountLQ = 0;

	glm::vec2 centre(3, 3);

	glm::vec2 mapSize(2000, 2000);
	glm::vec2 chunkSize(100, 100);

	for (int i = 0; i < 7; i++)
	{
		for (int j = 0; j < 7; j++)
		{
			glm::vec2 start(i*chunkSize.x, j*chunkSize.y);
			glm::vec2 end((i+1)*chunkSize.x, (j+1)*chunkSize.y);

			glm::vec2 distanceVector = glm::abs(glm::vec2(i, j) - centre);
			int distanceFromCentre = glm::max(distanceVector.x, distanceVector.y);

			if (distanceFromCentre == 3)
			{
				terrainBlocksLQ[blockCountLQ].create(&shader, &uniformLocations);
				terrainBlocksLQ[blockCountLQ].generate(start, end, mapSize, radius, 10);

				blockCountLQ++;
			}
			else if (distanceFromCentre == 2)
			{
				terrainBlocksMQ[blockCountMQ].create(&shader, &uniformLocations);
				terrainBlocksMQ[blockCountMQ].generate(start, end, mapSize, radius, 5);

				blockCountMQ++;
			}
			else if (distanceFromCentre <= 1) {

				terrainBlocksHQ[blockCountHQ].create(&shader, &uniformLocations);
				terrainBlocksHQ[blockCountHQ].generate(start, end, mapSize, radius, 1);
				blockCountHQ++;
			}
		}
	}
}