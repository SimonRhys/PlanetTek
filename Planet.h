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

#define PI 3.14159265358979323846
#define TWO_PI 6.28318530717958647693

class Planet
{

public:

	Planet(float radius);
	Planet(float radius, std::string heightmapFP);
	~Planet();

	void draw(glm::mat4 proj, glm::mat4 view);
	void loadTextures(std::string filePath);
	void setPlayerCamera(glm::vec3 *playerCamera);
	void update(float dt);


private:
	void createShaderProgram();
	void generate();
	void reloadLODs();
	bool intersect(glm::vec2 startPoint, glm::vec2 endList, glm::vec2 v);
	bool intersect(std::vector<glm::vec2> startList, std::vector<glm::vec2> endList, glm::vec2 v);

	bool reloading;

	float radius;

	const static int MAX_TERRAIN_BLOCKS_HQ = 10;
	const static int MAX_TERRAIN_BLOCKS_MQ = 17;
	const static int MAX_TERRAIN_BLOCKS_LQ = 25;
	const static int MAX_TERRAIN_BLOCKS_TOTAL = MAX_TERRAIN_BLOCKS_HQ + MAX_TERRAIN_BLOCKS_MQ + MAX_TERRAIN_BLOCKS_LQ;
	const static int LOD = 100;

	GLuint texture;

	glm::vec2 CHUNK_SIZE = glm::vec2(5, 5);

	Heightmap heightmap;

	Shader shader;

	std::map<std::string, GLuint> uniformLocations;

	std::vector<TerrainBlock*> lodMap;
	std::vector<std::pair<int, int>> regenMap;
	std::vector<std::pair<int, glm::vec2>> genMap;

	TerrainBlock terrainBlocks[MAX_TERRAIN_BLOCKS_TOTAL];

	glm::vec3 *playerCamera;

};

