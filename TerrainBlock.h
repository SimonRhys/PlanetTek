#pragma once

#include <map>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"
#include "Heightmap.h"
#include "Particles.h"

#define PI 3.14159265358979323846
#define TWO_PI 6.28318530717958647693

class TerrainBlock
{



public:

	TerrainBlock();
	~TerrainBlock();

	void draw(glm::mat4 proj, glm::mat4 view, int lod);
	void generate(glm::vec2 start, glm::vec2 end, Heightmap *heightmap, float radius);
	void markUnused();
	void update(float dt);

	bool isUsed();

	glm::vec2 getStartPoint();
	glm::vec2 getEndPoint();

	const static int LOW_QUALITY = 4;
	const static int MED_QUALITY = 2;
	const static int HIGH_QUALITY = 1;

private:
	void generateVertices(glm::vec2 start, glm::vec2 end, Heightmap *heightmap, float radius);
	void createVBO(glm::vec2 start, glm::vec2 end, std::vector<glm::vec3> *vertexList, int lod);

	glm::vec3 mapOctohedronToSphere(glm::vec2 coords, Heightmap *heightmap, float radius);
	glm::vec3 lerp(glm::vec3 v1, glm::vec3 v2, float p);
	glm::vec2 rotate(glm::vec2 v, float a);


	bool inUse;

	GLuint vaoLow;
	GLuint vboLow;

	GLuint vaoMed;
	GLuint vboMed;

	GLuint vaoHigh;
	GLuint vboHigh;

	int LOD;

	std::vector<glm::vec3> verticesLow;
	std::vector<glm::vec3> verticesMed;
	std::vector<glm::vec3> verticesHigh;

	glm::vec2 startPoint;
	glm::vec2 endPoint;

	Particles particles;

};