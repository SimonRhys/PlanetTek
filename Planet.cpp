#include "Planet.h"


//Public
Planet::Planet(float radius)
{
	this->heightModifier = 1000;
	this->radius = radius;
	this->reloading = false;
	this->seaLevel = 1024 * 1050;
	this->time = 0;
	heightmap.setHeightModifier(heightModifier);
	heightmap.create(64);
	terrainShader.setRadius(radius);
	terrainShader.setSeaLevel(seaLevel);
	generate();
	initThreads();
}

Planet::Planet(float radius, std::string heightmapFP)
{
	this->heightModifier = radius / 720;
	this->radius = radius;
	this->reloading = false;
	this->seaLevel = 1024 * 525;
	this->time = 0;
	this->sunPosition = glm::vec3(-radius*1.5, 0, 0);
	heightmap.setHeightModifier(heightModifier); 
	heightmap.create(22500000/256);
	//heightmap.load(heightmapFP);
	terrainShader.setRadius(radius);
	terrainShader.setSeaLevel(seaLevel);
	generate();	
	initThreads();

	entityShader.createShader("modelVert.sh", GL_VERTEX_SHADER);
	entityShader.createShader("modelFrag.sh", GL_FRAGMENT_SHADER);
	entityShader.createProgram();
}

void Planet::draw(glm::mat4 proj, glm::vec3 viewPos, glm::vec2 viewRotation) 
{
	glm::mat4 view = glm::rotate(viewRotation.x, glm::vec3(1, 0, 0));
	view = view * glm::rotate(viewRotation.y, glm::vec3(0, 1, 0));
	view = view * glm::translate(viewPos);

	float distance = 2.f * (glm::length(viewPos) - seaLevel);
	glm::vec3 dir = glm::normalize(viewPos);
	
	glm::mat4 reflectionView = glm::rotate(-viewRotation.x, glm::vec3(1, 0, 0));
	reflectionView = reflectionView * glm::rotate(viewRotation.y, glm::vec3(0, 1, 0));
	reflectionView = reflectionView * glm::translate(viewPos-dir*distance);

	float waveSpeed = time * 0.02;
	terrainShader.setWaveSpeed(waveSpeed);
	terrainShader.setSunPosition(sunPosition);

	//DRAW EVERYTHING BUT WATER FOR REFLECTION
	terrainShader.calcReflection(1);
	terrainShader.calcRefraction(0);
	terrainShader.drawSea(0);
	terrainShader.use(proj, viewPos, viewRotation);
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	for (int i = 0; i < MAX_TERRAIN_BLOCKS_TOTAL; i++)
	{
		terrainBlocks[i].draw(proj, reflectionView, *playerCamera);
	}

	glUseProgram(entityShader.getShaderProgram());

	GLuint p = glGetUniformLocation(entityShader.getShaderProgram(), "projection");
	GLuint v = glGetUniformLocation(entityShader.getShaderProgram(), "view");
	GLuint m = glGetUniformLocation(entityShader.getShaderProgram(), "model");

	glUniformMatrix4fv(p, 1, GL_FALSE, glm::value_ptr(proj));
	glUniformMatrix4fv(v, 1, GL_FALSE, glm::value_ptr(reflectionView));

	glm::mat4 model = glm::translate(glm::vec3(0, radius + 500, 0)); //-(radius+500)
	model = model * glm::scale(glm::mat4(1), glm::vec3(50));

	glUniformMatrix4fv(m, 1, GL_FALSE, glm::value_ptr(model));

	treeModel.draw(entityShader);

	skybox.draw(proj, reflectionView);

	//DRAW EVERYTHING BUT WATER FOR REFRACTION
	terrainShader.calcReflection(0);
	terrainShader.calcRefraction(1);
	terrainShader.drawSea(0);
	terrainShader.use(proj, viewPos, viewRotation);
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	for (int i = 0; i < MAX_TERRAIN_BLOCKS_TOTAL; i++)
	{
		terrainBlocks[i].draw(proj, view, *playerCamera);
	}

	glUseProgram(entityShader.getShaderProgram());

	glUniformMatrix4fv(p, 1, GL_FALSE, glm::value_ptr(proj));
	glUniformMatrix4fv(v, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(m, 1, GL_FALSE, glm::value_ptr(model));

	treeModel.draw(entityShader);

	skybox.draw(proj, view);

	//DRAW EVERYTHING
	terrainShader.calcReflection(0);
	terrainShader.calcRefraction(0);
	terrainShader.drawSea(1);
	terrainShader.use(proj, viewPos, viewRotation);
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	for (int i = 0; i < MAX_TERRAIN_BLOCKS_TOTAL; i++)
	{
		terrainBlocks[i].draw(proj, view, *playerCamera);
	}


	glUseProgram(entityShader.getShaderProgram());

	glUniformMatrix4fv(p, 1, GL_FALSE, glm::value_ptr(proj));
	glUniformMatrix4fv(v, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(m, 1, GL_FALSE, glm::value_ptr(model));

	treeModel.draw(entityShader);

	skybox.draw(proj, view);
	particles.draw();


}

void Planet::loadTexture(std::string filePath, std::string textureName)
{
	terrainShader.loadTexture(filePath, textureName);
}

void Planet::loadSkyboxDayTextures(std::vector<std::string> filePaths)
{
	skybox.loadDayTextures(filePaths);
}

void Planet::loadSkyboxNightTextures(std::vector<std::string> filePaths)
{
	skybox.loadNightTextures(filePaths);
}

void Planet::regenerate()
{
	generate();
}

void Planet::setHeightModifier(float hm)
{
	this->heightModifier = hm;
	heightmap.setHeightModifier(heightModifier);
}

void Planet::setPlayerCamera(glm::vec3 *playerCamera)
{
	this->playerCamera = playerCamera;
	terrainShader.setCamera(playerCamera);
}

void Planet::setRadius(float r)
{
	this->radius = r;
	terrainShader.setRadius(r);
}

void Planet::setRenderMode(RenderMode rm)
{
	this->currRenderMode = rm;
	generate();
}

void Planet::setSeaLevel(float sl)
{
	this->seaLevel = sl;
	terrainShader.setSeaLevel(sl);
}

void Planet::setWindowSize(float *w, float *h)
{
	terrainShader.init(w, h);
	particles.init(w, h);
}

void Planet::update(float dt)
{
	time += dt;

	skybox.update(dt);
	particles.update(dt);

	if (dayTime)
	{
		if (time > dayLength)
		{
			time = 0;
			dayTime = false;
		}

		float t = (time * glm::pi<float>() + glm::pi<float>()/8) / dayLength;
		sunPosition.x = glm::cos(t) * radius * 1.5;
		sunPosition.y = glm::sin(t) * radius * 1.5;
	}
	else
	{
		if (time > nightLength)
		{
			time = 0;
			dayTime = true;
		}

		float t = (time * glm::pi<float>() + glm::pi<float>() / 8) / nightLength;
		sunPosition.x = glm::cos(t) * radius * 1.5;
		sunPosition.y = glm::sin(t) * radius * 1.5;
	}


	int newBlock = currBlock;
	glm::vec3 cam = -*playerCamera;
	cam = glm::normalize(cam) * radius;
	for (int i = 0; i < MAX_TERRAIN_BLOCKS_TOTAL; i++)
	{
		if (!terrainBlocks[i].isUsed())
		{
			continue;
		}

		if (terrainBlocks[i].insideBlock(cam, radius))
		{
			newBlock = i;
		}

		terrainBlocks[i].update(dt);
	}

	/*if (newBlock != currBlock)
	{
		if (currBlock > 0)
		{
			glm::vec2 currStart = terrainBlocks[currBlock].getStartPoint();
			glm::vec2 newStart = terrainBlocks[newBlock].getStartPoint();

			glm::vec2 diff = currStart - newStart;

			if (diff.x > 0)
			{
				std::cout << "QUEUE LOAD BLOCKS TO LEFT" << std::endl;
			}
			else if (diff.x < 0)
			{
				std::cout << "QUEUE LOAD BLOCKS TO RIGHT" << std::endl;
			}

			if (diff.y > 0)
			{
				std::cout << "QUEUE LOAD BLOCKS TO UP" << std::endl;
			}
			else if (diff.y < 0)
			{
				std::cout << "QUEUE LOAD BLOCKS TO DOWN" << std::endl;
			}
		}

		currBlock = newBlock;
		std::cout << "CURRENT BLOCK " << currBlock << std::endl;
	}*/
}

Planet::~Planet()
{
}

//Private
void Planet::generate()
{
	if (currRenderMode == PARTIAL)
	{
		for (int i = 0; i < MAX_TERRAIN_BLOCKS_TOTAL; i++)
		{
			terrainBlocks[i].markUnused();
		}

		glm::vec2 centre(heightmap.getWidth() / 2, heightmap.getHeight() / 2);

		std::vector<glm::vec4> startEndPoints;
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

				startEndPoints.push_back(glm::vec4(start, end));

				blockCount++;
			}
		}

		int blockPos = 0;
		bool threadsFinished = false;

		while (blockPos < blockCount || !threadsFinished)
		{
			for (int j = 0; j < MAX_THREADS; j++)
			{
				if (threadStatus[j] != FREE)
				{
					continue;
				}

				if (blockPos >= blockCount)
				{
					break;
				}

				glm::vec2 start = glm::vec2(startEndPoints[blockPos].x, startEndPoints[blockPos].y);
				glm::vec2 end = glm::vec2(startEndPoints[blockPos].z, startEndPoints[blockPos].w);

				terrainThreads[j] = std::thread(&Planet::loadTerrainBlock, this, j, blockPos, start, end);
				threadStatus[j] = INITIALISED;
				threadToTerrain[j] = blockPos;
				terrainThreads[j].detach();
				blockPos++;
			}

			threadsFinished = true;
			for (int j = 0; j < MAX_THREADS; j++)
			{
				if (threadStatus[j] == FINISHED)
				{
					terrainBlocks[threadToTerrain[j]].createVBO();
					threadStatus[j] = FREE;
				}

				threadsFinished = threadsFinished && threadStatus[j] == FREE;
			}
		}
	}
	else
	{
		for (int i = 0; i < MAX_TERRAIN_BLOCKS_TOTAL; i++)
		{
			terrainBlocks[i].markUnused();
		}

		glm::vec2 start = glm::vec2(heightmap.getWidth() / 2);
		//glm::vec2 start = glm::vec2(0);
		terrainBlocks[0].generate(start, start + BLOCK_SIZE, &heightmap, radius);
	}
}

void Planet::loadTerrainBlock(int threadID, int blockPos, glm::vec2 start, glm::vec2 end)
{
	threadStatus[threadID] = RUNNING;
	terrainBlocks[blockPos].generate(start, end, &heightmap, radius);
	lodMap.push_back(&terrainBlocks[blockPos]);
	threadStatus[threadID] = FINISHED;
}

/*void Planet::updateLODBlocks()
{
	return; 

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
}*/

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

				terrainBlocks[j].generate(startPoint, endPoint, &heightmap, radius);
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

				terrainBlocks[j].generate(startPoint, endPoint, &heightmap, radius);
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

void Planet::initThreads()
{
	for (int i = 0; i < MAX_THREADS; i++)
	{
		threadStatus[i] = FREE;
	}
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