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
	void draw(glm::mat4 proj, glm::mat4 view, float radius, int lod);
	void generate(glm::vec3 start, glm::vec3 end, Heightmap *heightmap, float radius, int lod);
	void generate(std::vector<glm::vec3> start, std::vector<glm::vec3> end, Heightmap *heightmap, float radius, int lod);
	
	bool intersect(glm::vec3 p);

private:
	void generateVertices(glm::vec3 start, glm::vec3 end, Heightmap *heightmap, float radius, int lod);

	glm::vec3 mapCubeToSphere(glm::vec3 coords, float radius);

	bool inUse;

	GLuint vao;
	GLuint vbo;
	GLuint ebo;

	Shader *shader;

	std::map<std::string, GLuint> *uniformLocations;

	std::vector<glm::vec3> vertices;
	std::vector<GLuint> indices;

};

