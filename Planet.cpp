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
	createShaderProgram();
	createFBOs();
	generate();
}

Planet::Planet(float radius, std::string heightmapFP)
{
	this->heightModifier = 3500; //radius / heightmap.getSize()
	this->radius = radius;
	this->reloading = false;
	this->seaLevel = 1024 * 525;
	this->time = 0;
	heightmap.setHeightModifier(heightModifier); 
	heightmap.load(heightmapFP);
	createShaderProgram();
	createFBOs();
	generate();	
}

void Planet::createFBOs()
{
	glGenFramebuffers(1, &reflectionFBO);
	glGenFramebuffers(1, &refractionFBO);

	glBindFramebuffer(GL_FRAMEBUFFER, reflectionFBO);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	reflectionTextureID = createTextureAttachment(reflectionResolution.x, reflectionResolution.y);
	reflectionDepthBuffer = createDepthBufferAttachment(reflectionResolution.x, reflectionResolution.y);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "REFLECTION FRAMEBUFFER NOT COMPLETE" << std::endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, refractionFBO);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	refractionTextureID = createTextureAttachment(refractionResolution.x, refractionResolution.y);
	refractionDepthTextureID = createDepthTextureAttachment(refractionResolution.x, refractionResolution.y);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "REFRACTION FRAMEBUFFER NOT COMPLETE" << std::endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glUseProgram(shader.getShaderProgram());
	glUniform1i(glGetUniformLocation(shader.getShaderProgram(), "reflectionTexture"), numTexturesLoaded);
	textures[numTexturesLoaded++] = reflectionTextureID;
	glUniform1i(glGetUniformLocation(shader.getShaderProgram(), "refractionTexture"), numTexturesLoaded);
	textures[numTexturesLoaded++] = refractionTextureID;
	glUseProgram(0);
}


GLuint Planet::createTextureAttachment(int width, int height)
{
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture, 0);

	return texture;
}

GLuint Planet::createDepthTextureAttachment(int width, int height)
{
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texture, 0);

	return texture;
}

GLuint Planet::createDepthBufferAttachment(int width, int height)
{
	GLuint depthBuffer;
	glGenRenderbuffers(1, &depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

	return depthBuffer;
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

	glEnable(GL_CLIP_DISTANCE0);

	float waveSpeed = time * 0.02;

	glUseProgram(shader.getShaderProgram());

	glUniformMatrix4fv(uniformLocations.at("projection"), 1, GL_FALSE, glm::value_ptr(proj));
	glUniformMatrix4fv(uniformLocations.at("view"), 1, GL_FALSE, glm::value_ptr(reflectionView));
	glUniformMatrix4fv(uniformLocations.at("model"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1)));

	glUniform3f(uniformLocations.at("lightPos"), 0, radius * 1.5, 0);
	glUniform3f(uniformLocations.at("lightColor"), 1.0f, 1.0f, 1.0f);
	glUniform3f(uniformLocations.at("eyePos"), -playerCamera->x, -playerCamera->y, -playerCamera->z);
	glUniform3f(uniformLocations.at("skyColour"), 0.298, 0.5, 0.5);
	
	glUniform1f(uniformLocations.at("seaLevel"), seaLevel);
	glUniform1f(uniformLocations.at("waveSpeed"), waveSpeed);

	glUniform1i(uniformLocations.at("renderSea"), 0);
	glUniform1i(uniformLocations.at("reflection"), 1);
	glUniform1i(uniformLocations.at("refraction"), 0);

	for (int i = 0; i < numTexturesLoaded; i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, textures[i]);
	}

	//DRAW EVERYTHING BUT WATER FOR REFLECTION
	glBindFramebuffer(GL_FRAMEBUFFER, reflectionFBO);
	glViewport(0, 0, reflectionResolution.x, reflectionResolution.y);
	// Clear the colorbuffer
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	for (int i = 0; i < MAX_TERRAIN_BLOCKS_TOTAL; i++)
	{
		terrainBlocks[i].draw(proj, reflectionView, TerrainBlock::HIGH_QUALITY);
	}

	glUseProgram(0);

	skybox.draw(proj, reflectionView);

	//DRAW EVERYTHING BUT WATER FOR REFRACTION

	glUseProgram(shader.getShaderProgram());

	glUniformMatrix4fv(uniformLocations.at("projection"), 1, GL_FALSE, glm::value_ptr(proj));
	glUniformMatrix4fv(uniformLocations.at("view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(uniformLocations.at("model"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1)));

	glUniform3f(uniformLocations.at("lightPos"), 0, radius * 1.5, 0);
	glUniform3f(uniformLocations.at("lightColor"), 1.0f, 1.0f, 1.0f);
	glUniform3f(uniformLocations.at("eyePos"), -playerCamera->x, -playerCamera->y, -playerCamera->z);
	glUniform3f(uniformLocations.at("skyColour"), 0.298, 0.5, 0.5);

	glUniform1f(uniformLocations.at("seaLevel"), seaLevel);
	glUniform1f(uniformLocations.at("waveSpeed"), waveSpeed);

	glUniform1i(uniformLocations.at("renderSea"), 0);
	glUniform1i(uniformLocations.at("reflection"), 0);
	glUniform1i(uniformLocations.at("refraction"), 1);

	for (int i = 0; i < numTexturesLoaded; i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, textures[i]);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, refractionFBO);
	glViewport(0, 0, refractionResolution.x, refractionResolution.y);
	// Clear the colorbuffer
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	for (int i = 0; i < MAX_TERRAIN_BLOCKS_TOTAL; i++)
	{
		terrainBlocks[i].draw(proj, view, TerrainBlock::HIGH_QUALITY);
	}

	glUseProgram(0);

	skybox.draw(proj, view);

	glDisable(GL_CLIP_DISTANCE0);

	//DRAW EVERYTHING
	glUseProgram(shader.getShaderProgram());

	glUniformMatrix4fv(uniformLocations.at("projection"), 1, GL_FALSE, glm::value_ptr(proj));
	glUniformMatrix4fv(uniformLocations.at("view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(uniformLocations.at("model"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1)));

	glUniform3f(uniformLocations.at("lightPos"), 0, radius * 1.5, 0);
	glUniform3f(uniformLocations.at("lightColor"), 1.0f, 1.0f, 1.0f);
	glUniform3f(uniformLocations.at("eyePos"), -playerCamera->x, -playerCamera->y, -playerCamera->z);
	glUniform3f(uniformLocations.at("skyColour"), 0.298, 0.5, 0.5);

	glUniform1f(uniformLocations.at("seaLevel"), seaLevel);
	glUniform1f(uniformLocations.at("waveSpeed"), waveSpeed);

	glUniform1i(uniformLocations.at("renderSea"), 1);
	glUniform1i(uniformLocations.at("reflection"), 0);
	glUniform1i(uniformLocations.at("refraction"), 0);

	for (int i = 0; i < numTexturesLoaded; i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, textures[i]);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, 1200, 900);
	// Clear the colorbuffer
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	for (int i = 0; i < MAX_TERRAIN_BLOCKS_TOTAL; i++)
	{
		terrainBlocks[i].draw(proj, view, TerrainBlock::HIGH_QUALITY);
	}

	glUseProgram(0);

	skybox.draw(proj, view);
}

void Planet::loadTexture(std::string filePath, std::string textureName)
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

	glBindTexture(GL_TEXTURE_2D, textures[numTexturesLoaded]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glUseProgram(shader.getShaderProgram());
	glUniform1i(glGetUniformLocation(shader.getShaderProgram(), textureName.c_str()), numTexturesLoaded);
	glUseProgram(0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//glGenerateMipmap(GL_TEXTURE_2D);

	numTexturesLoaded++;
	SOIL_free_image_data(image);
}

void Planet::loadSkybox(std::vector<std::string> filePaths)
{
	skybox.loadTextures(filePaths);
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
}

void Planet::setRadius(float r)
{
	this->radius = r;
}

void Planet::setRenderMode(RenderMode rm)
{
	this->currRenderMode = rm;
	generate();
}

void Planet::setSeaLevel(float sl)
{
	this->seaLevel = sl;
}

void Planet::update(float dt)
{
	time += dt;
	if (time > 600)
	{
		time = 0;
	}

	updateLODBlocks();
	for (int i = 0; i < MAX_TERRAIN_BLOCKS_TOTAL; i++)
	{
		if (terrainBlocks[i].isUsed())
		{
			terrainBlocks[i].update(dt);
		}
	}
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
	uniformLocations["eyePos"] = glGetUniformLocation(shader.getShaderProgram(), "eyePos");
	uniformLocations["seaLevel"] = glGetUniformLocation(shader.getShaderProgram(), "seaLevel");
	uniformLocations["waveSpeed"] = glGetUniformLocation(shader.getShaderProgram(), "waveSpeed");
	uniformLocations["renderSea"] = glGetUniformLocation(shader.getShaderProgram(), "renderSea");
	uniformLocations["skyColour"] = glGetUniformLocation(shader.getShaderProgram(), "skyColour");
	uniformLocations["reflectionTexture"] = glGetUniformLocation(shader.getShaderProgram(), "reflectionTexture");
	uniformLocations["refractionTexture"] = glGetUniformLocation(shader.getShaderProgram(), "refractionTexture");
	uniformLocations["reflection"] = glGetUniformLocation(shader.getShaderProgram(), "reflection");
	uniformLocations["refraction"] = glGetUniformLocation(shader.getShaderProgram(), "refraction");
}

void Planet::updateLODBlocks()
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

		for (int i = 0; i <= 0; i++) // -3 -> 3
		{
			for (int j = 0; j <= 0; j++) // -3 -> 3
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

				terrainBlocks[blockCount].generate(start, end, &heightmap, radius);
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

		glm::vec2 start = glm::vec2(heightmap.getWidth() / 2, heightmap.getHeight() / 2);
		terrainBlocks[0].generate(start, start+glm::vec2(256, 256), &heightmap, radius);
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