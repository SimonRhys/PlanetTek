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
	void setPlayerCamera(glm::vec3 *playerCamera);
	void update(float dt);


private:
	void createShaderProgram();
	void generate();
	void ensureLimits(	std::vector<glm::vec3> *startList, std::vector<glm::vec3> *endList, char toCheck);
	bool intersect(std::vector<glm::vec3> startList, std::vector<glm::vec3> endList, glm::vec3 v);

	float radius;

	const static int MAX_TERRAIN_BLOCKS_HQ = 10;
	const static int MAX_TERRAIN_BLOCKS_MQ = 17;
	const static int MAX_TERRAIN_BLOCKS_LQ = 25;
	const static int LOD = 100;

	glm::vec2 CHUNK_SIZE = glm::vec2(5, 5);

	Heightmap heightmap;

	Shader shader;

	std::map<std::string, GLuint> uniformLocations;
	std::map<int, TerrainBlock*> lodMap;
	std::map<int, int> regenMap;

	TerrainBlock terrainBlocksHQ[MAX_TERRAIN_BLOCKS_HQ];
	TerrainBlock terrainBlocksMQ[MAX_TERRAIN_BLOCKS_MQ];
	TerrainBlock terrainBlocksLQ[MAX_TERRAIN_BLOCKS_LQ];

	glm::vec3 *playerCamera;

};

