#pragma once

#include <map>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"
#include "Heightmap.h"

#define PI 3.14159265358979323846
#define TWO_PI 6.28318530717958647693

class TerrainBlock
{



public:

	TerrainBlock();
	~TerrainBlock();

	void draw(glm::mat4 proj, glm::mat4 view, glm::vec3 cameraPos);
	void createVBO();
	void generate(glm::vec2 start, glm::vec2 end, Heightmap *heightmap, float radius);
	void markUnused();
	void setWindowSize(GLfloat *width, GLfloat *height);
	void update(float dt);

	bool insideBlock(glm::vec3 point, float radius);


	bool isUsed();

	glm::vec2 getStartPoint();
	glm::vec2 getEndPoint();

	glm::vec3 getCentrePoint();

	const static int LOW_QUALITY = 16;
	const static int MED_QUALITY = 8;
	const static int HIGH_QUALITY = 1;

private:
	float calcGreatCircleDistance(float phi1, float lambda1, float phi2, float lambda2, float radius);
	float calcGreatCircleDistance(glm::vec3 p1, glm::vec3 p2, float radius);
	void createVertexLists(glm::vec2 start, glm::vec2 end, std::vector<glm::vec3> *vertexList, int lod);
	void generateVertices(glm::vec2 start, glm::vec2 end, Heightmap *heightmap, float radius);

	glm::vec3 mapOctohedronToSphere(glm::vec2 coords, Heightmap *heightmap, float radius);
	glm::vec3 lerp(glm::vec3 v1, glm::vec3 v2, float p);
	glm::vec2 rotate(glm::vec2 v, float a);
	glm::vec2 calcAngles(glm::vec3 p);

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

	glm::vec3 c1;
	glm::vec3 c2;
	glm::vec3 c3;
	glm::vec3 c4;
	glm::vec3 centrePoint;


	float radius;
};