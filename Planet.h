#pragma once

#include <map>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"
#include "TerrainBlock.h"

#define PI 3.14159265358979323846
#define TWO_PI 6.28318530717958647693



class Planet
{

public:

	enum class LOD_QUALITY { HIGH, MEDIUM, LOW };

	Planet(float radius);
	~Planet();

	void draw(glm::mat4 proj, glm::mat4 view);
	void setPlayerCamera(glm::vec3 *playerCamera);
	void update(float dt);


private:
	void generate();

	float radius;

	const static int MAX_TERRAIN_BLOCKS_HQ = 10;
	const static int MAX_TERRAIN_BLOCKS_MQ = 17;
	const static int MAX_TERRAIN_BLOCKS_LQ = 25;
	const static int CHUNK_SIZE = 100;
	const static int LOD = 100;





	Shader shader;

	std::map<std::string, GLuint> uniformLocations;

	LOD_QUALITY LODQuality;

	TerrainBlock terrainBlocksHQ[MAX_TERRAIN_BLOCKS_HQ];
	TerrainBlock terrainBlocksMQ[MAX_TERRAIN_BLOCKS_MQ];
	TerrainBlock terrainBlocksLQ[MAX_TERRAIN_BLOCKS_LQ];

	glm::vec3 *playerCamera;

};

