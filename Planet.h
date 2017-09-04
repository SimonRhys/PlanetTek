#pragma once

#include <map>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <SOIL/SOIL.h>
#include <thread>
#include "Shader.h"
#include "TerrainBlock.h"
#include "TerrainShader.h"
#include "Heightmap.h"
#include "Skybox.h"
#include "Model.h"
#include "Particles.h"

class Planet
{

public:
	enum RenderMode { PARTIAL, WHOLE };
	enum ThreadStatus { FREE, INITIALISED, RUNNING, FINISHED };

	Planet(float radius);
	Planet(float radius, std::string heightmapFP);
	~Planet();

	void draw(glm::mat4 proj, glm::vec3 viewPos, glm::vec2 viewRotation);
	void loadTexture(std::string filePath, std::string textureName);
	void loadSkyboxDayTextures(std::vector<std::string> filePaths);
	void loadSkyboxNightTextures(std::vector<std::string> filePaths);
	void regenerate();
	void setPlayerCamera(glm::vec3 *playerCamera);
	void setRenderMode(RenderMode rm);
	void setHeightModifier(float hm);
	void setRadius(float r);
	void setSeaLevel(float sl);
	void setWindowSize(float *w, float *h);
	void update(float dt);


private:

	void generate();
	void loadTerrainBlock(int threadID, int blockPos, glm::vec2 start, glm::vec2 end);
	void updateLODBlocks();
	void reloadLODs();
	void initThreads();
	bool intersect(glm::vec2 startPoint, glm::vec2 endList, glm::vec2 v);
	bool intersect(std::vector<glm::vec2> startList, std::vector<glm::vec2> endList, glm::vec2 v);

	bool reloading;

	float heightModifier;
	float radius;
	float seaLevel;
	float time;
	float dayLength = 60;
	float nightLength = 45;

	bool dayTime = true;


	const static int MAX_THREADS = 16;
	const static int MAX_TERRAIN_BLOCKS_HQ = 10;
	const static int MAX_TERRAIN_BLOCKS_MQ = 17;
	const static int MAX_TERRAIN_BLOCKS_LQ = 25;
	const static int MAX_TERRAIN_BLOCKS_TOTAL = MAX_TERRAIN_BLOCKS_HQ + MAX_TERRAIN_BLOCKS_MQ + MAX_TERRAIN_BLOCKS_LQ;

	std::thread terrainThreads[MAX_THREADS];
	std::map<int, ThreadStatus> threadStatus;
	std::map<int, int> threadToTerrain;

	glm::vec2 BLOCK_SIZE = glm::vec2(96, 96); 

	Heightmap heightmap;

	Particles particles;

	TerrainShader terrainShader;
	Shader entityShader;
	Skybox skybox;
	Model treeModel = Model("obj/nanosuit/nanosuit.obj");

	std::map<std::string, GLuint> uniformLocations;

	std::vector<TerrainBlock*> lodMap;
	std::vector<std::pair<int, int>> regenMap;
	std::vector<std::pair<int, glm::vec2>> genMap;

	TerrainBlock terrainBlocks[MAX_TERRAIN_BLOCKS_TOTAL];
	int currBlock = -1;

	glm::vec3 *playerCamera;
	glm::vec3 sunPosition;

	RenderMode currRenderMode = PARTIAL;

	GLuint reflectionFBO;
	GLuint refractionFBO;
	GLuint reflectionTextureID;
	GLuint refractionTextureID;
	GLuint refractionDepthTextureID;
	GLuint reflectionDepthBuffer;

	glm::vec2 reflectionResolution = glm::vec2(800, 600);
	glm::vec2 refractionResolution = glm::vec2(800, 600);

};

