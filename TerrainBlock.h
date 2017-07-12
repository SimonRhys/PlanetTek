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

	void create(Shader *shader, std::map<std::string, GLuint> *uniformLocations);
	void draw(glm::mat4 proj, glm::mat4 view, float radius);
	void draw(glm::mat4 proj, glm::mat4 view, float radius, GLuint texture);
	void generate(glm::vec2 start, glm::vec2 end, Heightmap *heightmap, float radius, int lod);
	void markUnused();

	bool isUsed();

	glm::vec2 getStartPoint();
	glm::vec2 getEndPoint();

private:
	void generateVertices(glm::vec2 start, glm::vec2 end, Heightmap *heightmap, float radius, int lod);

	glm::vec3 mapCubeToSphere(glm::vec3 coords, float radius);
	glm::vec3 mapOctohedronToSphere(glm::vec2 coords, Heightmap *heightmap, float radius);
	glm::vec3 lerp(glm::vec3 v1, glm::vec3 v2, float p);
	glm::vec2 rotate(glm::vec2 v, float a);


	bool inUse;

	GLuint vao;
	GLuint vbo;
	GLuint ebo;

	int LOD;

	Shader *shader;

	std::map<std::string, GLuint> *uniformLocations;

	std::vector<glm::vec3> vertices;
	std::vector<GLuint> indices;

	glm::vec2 startPoint;
	glm::vec2 endPoint;

};