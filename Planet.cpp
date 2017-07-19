#include "Planet.h"


//Public
Planet::Planet(float radius)
{
	this->radius = radius;
	this->reloading = false;
	heightmap.create(64);
	createShaderProgram();
	generate();
}

Planet::Planet(float radius, std::string heightmapFP)
{
	this->radius = radius;
	this->reloading = false;
	heightmap.load(heightmapFP);
	heightmap.setHeightModifier(radius/512);
	createShaderProgram();
	generate();	
}

void Planet::draw(glm::mat4 proj, glm::mat4 view) 
{
	glUseProgram(shader.getShaderProgram());

	glUniformMatrix4fv(uniformLocations.at("projection"), 1, GL_FALSE, glm::value_ptr(proj));
	glUniformMatrix4fv(uniformLocations.at("view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(uniformLocations.at("model"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1)));

	glUniform3f(uniformLocations.at("lightPos"), -radius * 1.5, radius * 1.5, radius * 1.5);
	glUniform3f(uniformLocations.at("lightColor"), 1.0f, 1.0f, 1.0f);
	
	glUniform1f(uniformLocations.at("seaLevel"), 1024 * 1100);

	for (int i = 0; i < MAX_TERRAIN_BLOCKS_TOTAL; i++)
	{
		terrainBlocks[i].draw(proj, view, radius);
	}

	glUseProgram(0);
}

void Planet::loadTexture(std::string filePath)
{
	int w, h, channels;
	unsigned char *image = SOIL_load_image(filePath.c_str(), &w, &h, &channels, SOIL_LOAD_RGB);
	if (image == 0)
	{
		std::cout << "SOIL failed to load Planet texture!" << std::endl;
		return;
	}
	else
	{
		std::cout << "SOIL loaded Planet texture!" << std::endl;
	}

	glGenTextures(1, &textures[numTexturesLoaded]);

	glActiveTexture(GL_TEXTURE0 + numTexturesLoaded);
	glBindTexture(GL_TEXTURE_2D, textures[numTexturesLoaded]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	std::string loc = "texSampler" + std::to_string(numTexturesLoaded);
	glUseProgram(shader.getShaderProgram());
	glUniform1i(glGetUniformLocation(shader.getShaderProgram(), loc.c_str()), numTexturesLoaded);
	glUseProgram(0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenerateMipmap(GL_TEXTURE_2D);

	numTexturesLoaded++;
	SOIL_free_image_data(image);
}

void Planet::setPlayerCamera(glm::vec3 *playerCamera)
{
	this->playerCamera = playerCamera;
}

void Planet::setRenderMode(RenderMode rm)
{
	this->currRenderMode = rm;
	generate();
}

void Planet::update(float dt)
{
	updateLODBlocks();
}

Planet::~Planet()
{
	glDeleteTextures(numTexturesLoaded, textures);
}

//Private
void Planet::createShaderProgram()
{
	shader.createShader("planetVert.sh", GL_VERTEX_SHADER);
	shader.createShader("planetFrag.sh", GL_FRAGMENT_SHADER);
	shader.createShader("planetGeo.sh", GL_GEOMETRY_SHADER);
	shader.createProgram();

	uniformLocations["projection"] = glGetUniformLocation(shader.getShaderProgram(), "projection");
	uniformLocations["view"] = glGetUniformLocation(shader.getShaderProgram(), "view");
	uniformLocations["model"] = glGetUniformLocation(shader.getShaderProgram(), "model");
	uniformLocations["lightPos"] = glGetUniformLocation(shader.getShaderProgram(), "lightPos");
	uniformLocations["lightColor"] = glGetUniformLocation(shader.getShaderProgram(), "lightColor");
	uniformLocations["seaLevel"] = glGetUniformLocation(shader.getShaderProgram(), "seaLevel");
}

void Planet::updateLODBlocks()
{
	if (reloading || currRenderMode == WHOLE)
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
				if (i == 1)
				{
					TerrainBlock* tempBlocks[7];
					for (int k = 0; k < 7; k++)
					{
						tempBlocks[k] = lodMap[k];
					}

					for (int k = 0; k < lodMap.size() - 7; k++)
					{
						lodMap[k] = lodMap[k + 7];
					}

					for (int k = 0; k < 7; k++)
					{
						lodMap[lodMap.size() - 7 + k] = tempBlocks[k];

						//For the last 7 chunks we need to create the far LOD
						glm::vec2 modifier(0, BLOCK_SIZE.y);
						genMap.push_back(std::pair<int, glm::vec2>(lodMap.size() - 7 + k, modifier));
					}

					regenMap.push_back(std::pair<int, int>(9, MED_QUALITY));
					regenMap.push_back(std::pair<int, int>(10, MED_QUALITY));
					regenMap.push_back(std::pair<int, int>(11, MED_QUALITY));

					regenMap.push_back(std::pair<int, int>(30, HIGH_QUALITY));
					regenMap.push_back(std::pair<int, int>(31, HIGH_QUALITY));
					regenMap.push_back(std::pair<int, int>(32, HIGH_QUALITY));

					regenMap.push_back(std::pair<int, int>(37, MED_QUALITY));
					regenMap.push_back(std::pair<int, int>(38, MED_QUALITY));
					regenMap.push_back(std::pair<int, int>(39, MED_QUALITY));
				}
				else if (i == -1)
				{
					TerrainBlock* tempBlocks[7];
					for (int k = 0; k < 7; k++)
					{
						tempBlocks[k] = lodMap[lodMap.size() - 7 + k];
					}

					for (int k = lodMap.size() - 1; k > 6; k--)
					{
						lodMap[k] = lodMap[k - 7];
					}

					for (int k = 0; k < 7; k++)
					{
						lodMap[k] = tempBlocks[k];

						//For k = [0, 6] we need to create the far LOD
						glm::vec2 modifier(0, -BLOCK_SIZE.y);
						genMap.push_back(std::pair<int, glm::vec2>(k, modifier));
					}

					regenMap.push_back(std::pair<int, int>(9, MED_QUALITY));
					regenMap.push_back(std::pair<int, int>(10, MED_QUALITY));
					regenMap.push_back(std::pair<int, int>(11, MED_QUALITY));

					regenMap.push_back(std::pair<int, int>(16, HIGH_QUALITY));
					regenMap.push_back(std::pair<int, int>(17, HIGH_QUALITY));
					regenMap.push_back(std::pair<int, int>(18, HIGH_QUALITY));

					regenMap.push_back(std::pair<int, int>(37, MED_QUALITY));
					regenMap.push_back(std::pair<int, int>(38, MED_QUALITY));
					regenMap.push_back(std::pair<int, int>(39, MED_QUALITY));
				}

				if (j == -1)
				{
					TerrainBlock* tempBlocks[7];
					for (int k = 0; k < 7; k++)
					{
						tempBlocks[k] = lodMap[6 + k * 7];
					}

					for (int k = lodMap.size() - 1; k >= 0; k--)
					{
						if (k % 7 != 0)
						{
							lodMap[k] = lodMap[k - 1];
						}
					}

					for (int k = 0; k < 7; k++)
					{
						lodMap[k * 7] = tempBlocks[k];

						//For k = [0, 7, 14, 21...] we need to create the far LOD
						glm::vec2 modifier(-BLOCK_SIZE.x, 0);
						genMap.push_back(std::pair<int, glm::vec2>(k * 7, modifier));
					}


					regenMap.push_back(std::pair<int, int>(26, MED_QUALITY));
					regenMap.push_back(std::pair<int, int>(19, MED_QUALITY));
					regenMap.push_back(std::pair<int, int>(33, MED_QUALITY));

					regenMap.push_back(std::pair<int, int>(23, HIGH_QUALITY));
					regenMap.push_back(std::pair<int, int>(16, HIGH_QUALITY));
					regenMap.push_back(std::pair<int, int>(30, HIGH_QUALITY));

					regenMap.push_back(std::pair<int, int>(22, MED_QUALITY));
					regenMap.push_back(std::pair<int, int>(15, MED_QUALITY));
					regenMap.push_back(std::pair<int, int>(29, MED_QUALITY));
				}
				else if (i == 0 && j == 1)
				{
					TerrainBlock* tempBlocks[7];
					for (int k = 0; k < 7; k++)
					{
						tempBlocks[k] = lodMap[k * 7];
					}

					for (int k = 0; k < lodMap.size(); k++)
					{
						if (k % 7 != 6)
						{
							lodMap[k] = lodMap[k + 1];
						}
					}

					for (int k = 0; k < 7; k++)
					{
						lodMap[6 + k * 7] = tempBlocks[k];

						//For k = [0, 7, 14, 21...] we need to create the far LOD
						glm::vec2 modifier(BLOCK_SIZE.x, 0);
						genMap.push_back(std::pair<int, glm::vec2>(6 + k * 7, modifier));
					}
					regenMap.push_back(std::pair<int, int>(22, MED_QUALITY));
					regenMap.push_back(std::pair<int, int>(15, MED_QUALITY));
					regenMap.push_back(std::pair<int, int>(29, MED_QUALITY));

					regenMap.push_back(std::pair<int, int>(25, HIGH_QUALITY));
					regenMap.push_back(std::pair<int, int>(18, HIGH_QUALITY));
					regenMap.push_back(std::pair<int, int>(32, HIGH_QUALITY));

					regenMap.push_back(std::pair<int, int>(26, MED_QUALITY));
					regenMap.push_back(std::pair<int, int>(19, MED_QUALITY));
					regenMap.push_back(std::pair<int, int>(33, MED_QUALITY));
				}

				reloadLODs();
			}

			mapCounter++;
		}
	}
}

void Planet::generate()
{
	if (currRenderMode == PARTIAL)
	{
		for (int i = 0; i < MAX_TERRAIN_BLOCKS_TOTAL; i++)
		{
			terrainBlocks[i].markUnused();
		}

		glm::vec2 centre(heightmap.getWidth() / 2, heightmap.getHeight() / 2);

		int blockCount = 0;

		for (int i = -3; i <= 3; i++)
		{
			for (int j = -3; j <= 3; j++)
			{
				glm::vec2 start = centre;
				glm::vec2 end;


				start.x = centre.x + j*BLOCK_SIZE.x;
				start.y = centre.y + i*BLOCK_SIZE.y;

				end = start;
				end.x = start.x + BLOCK_SIZE.x;
				end.y = start.y + BLOCK_SIZE.y;

				glm::vec2 distanceVector = glm::abs(glm::vec2(j, i));
				int distanceFromCentre = glm::max(distanceVector.x, distanceVector.y);
				int lod;

				if (distanceFromCentre == 3)
				{
					lod = LOW_QUALITY;
				}
				else if (distanceFromCentre == 2)
				{
					lod = MED_QUALITY;
				}
				else
				{
					lod = HIGH_QUALITY;
				}

				terrainBlocks[blockCount].generate(start, end, &heightmap, radius, lod);
				lodMap.push_back(&terrainBlocks[blockCount]);
				blockCount++;
			}
		}
	}
	else
	{
		for (int i = 0; i < MAX_TERRAIN_BLOCKS_TOTAL; i++)
		{
			terrainBlocks[i].markUnused();
		}

		terrainBlocks[0].generate(glm::vec2(0, 0), glm::vec2(heightmap.getWidth(), heightmap.getHeight()), &heightmap, radius, MED_QUALITY);
	}
}

void Planet::reloadLODs()
{
	std::cout << "Reloading LODs..." << std::endl;

	reloading = true;

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

	for (int i = 0; i < genMap.size(); i++)
	{
		TerrainBlock* currBlock = lodMap[genMap[i].first];
		TerrainBlock* nextBlock = nullptr;

		if (genMap[i].second.y > 0)
		{
			nextBlock = lodMap[genMap[i].first-7];
		}
		else if (genMap[i].second.y < 0)
		{
			nextBlock = lodMap[genMap[i].first+7];
		}
		else if (genMap[i].second.x > 0)
		{
			nextBlock = lodMap[genMap[i].first - 1];
		}
		else if (genMap[i].second.x < 0)
		{
			nextBlock = lodMap[genMap[i].first + 1];
		}

		for (int j = 0; j < MAX_TERRAIN_BLOCKS_TOTAL; j++)
		{
			if (!terrainBlocks[j].isUsed())
			{
				glm::vec2 startPoint = nextBlock->getStartPoint();
				glm::vec2 endPoint = nextBlock->getEndPoint();

				startPoint += genMap[i].second;
				endPoint += genMap[i].second;

				terrainBlocks[j].generate(startPoint, endPoint, &heightmap, radius, LOW_QUALITY);
				currBlock->markUnused();

				lodMap[genMap[i].first] = &terrainBlocks[j];

				break;
			}
		}
	}

	regenMap.clear();
	genMap.clear();

	reloading = false;

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