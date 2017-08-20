#pragma once

#include <map>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <SOIL/SOIL.h>
#include "Shader.h"
#include "TerrainBlock.h"
#include "Heightmap.h"
#include "Skybox.h"

#define PI 3.14159265358979323846
#define TWO_PI 6.28318530717958647693

class Planet
{

public:
	enum RenderMode { PARTIAL, WHOLE };

	Planet(float radius);
	Planet(float radius, std::string heightmapFP);
	~Planet();

	void draw(glm::mat4 proj, glm::vec3 viewPos, glm::vec2 viewRotation);
	void loadTexture(std::string filePath, std::string textureName);
	void loadSkybox(std::vector<std::string> filePaths);
	void regenerate();
	void setPlayerCamera(glm::vec3 *playerCamera);
	void setRenderMode(RenderMode rm);
	void setHeightModifier(float hm);
	void setRadius(float r);
	void setSeaLevel(float sl);
	void update(float dt);


private:

	void createShaderProgram();
	void createFBOs();
	GLuint createTextureAttachment(int width, int height);
	GLuint createDepthTextureAttachment(int width, int height);
	GLuint createDepthBufferAttachment(int width, int height);
	void updateLODBlocks();
	void generate();
	void reloadLODs();
	bool intersect(glm::vec2 startPoint, glm::vec2 endList, glm::vec2 v);
	bool intersect(std::vector<glm::vec2> startList, std::vector<glm::vec2> endList, glm::vec2 v);

	bool reloading;

	float heightModifier;
	float radius;
	float seaLevel;
	float time;

	const static int LOW_QUALITY = 64;
	const static int MED_QUALITY = 5;
	const static int HIGH_QUALITY = 2;
	const static int MAX_TERRAIN_BLOCKS_HQ = 10;
	const static int MAX_TERRAIN_BLOCKS_MQ = 17;
	const static int MAX_TERRAIN_BLOCKS_LQ = 25;
	const static int MAX_TERRAIN_BLOCKS_TOTAL = MAX_TERRAIN_BLOCKS_HQ + MAX_TERRAIN_BLOCKS_MQ + MAX_TERRAIN_BLOCKS_LQ;

	GLuint textures[16];
	int numTexturesLoaded = 0;

	glm::vec2 BLOCK_SIZE = glm::vec2(256, 256);

	Heightmap heightmap;

	Shader shader;
	Skybox skybox;

	std::map<std::string, GLuint> uniformLocations;

	std::vector<TerrainBlock*> lodMap;
	std::vector<std::pair<int, int>> regenMap;
	std::vector<std::pair<int, glm::vec2>> genMap;

	TerrainBlock terrainBlocks[MAX_TERRAIN_BLOCKS_TOTAL];

	glm::vec3 *playerCamera;

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

