#pragma once

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <SOIL/SOIL.h>

#include "Shader.h"
#include "Mesh.h"


class Model
{
public:
	Model(std::string path);

	void draw(Shader shader);

	~Model();

private:
	void loadModel(std::string path);
	void processNode(aiNode *node, const aiScene *scene);
	Mesh processMesh(aiMesh *mesh, const aiScene *scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
	GLuint loadTexture(std::string filePath);

	std::vector<Mesh> meshes;
	std::string dir;
	bool loaded;
};

