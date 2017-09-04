#pragma once
#include <vector>

#include <assimp/Importer.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.h"

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;
};

struct Texture {
	unsigned int id;
	std::string type;
	aiString path;
};

class Mesh
{

public:
	Mesh(std::vector<Vertex> verts, std::vector<unsigned int> indices, std::vector<Texture> textures);

	void draw(Shader shader);

	~Mesh();

private:
	void setupMesh();


	std::vector<Vertex> verts;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	GLuint vao;
	GLuint vbo;
	GLuint ebo;
};

