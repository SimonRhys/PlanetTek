#include "Mesh.h"



Mesh::Mesh(std::vector<Vertex> verts, std::vector<unsigned int> indices, std::vector<Texture> textures)
{
	this->verts = verts;
	this->indices = indices;
	this->textures = textures;

	setupMesh();
}

void Mesh::draw(Shader shader)
{
	unsigned int diffuseCount = 1;
	unsigned int specularCount = 1;
	unsigned int normalCount = 1;
	unsigned int heightCount = 1;
	for (unsigned int i = 0; i < textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i); 
										  
		std::string number;
		std::string name = textures[i].type;
		if (name == "texture_diffuse")
			number = std::to_string(diffuseCount++); 
		else if (name == "texture_specular")
			number = std::to_string(specularCount++);
		else if (name == "texture_normal")
			number = std::to_string(normalCount++);
		else if (name == "texture_height")
			number = std::to_string(heightCount++);

		glUniform1i(glGetUniformLocation(shader.getShaderProgram(), (name + number).c_str()), i);
		glBindTexture(GL_TEXTURE_2D, textures[i].id);
	}

	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glActiveTexture(GL_TEXTURE0);
}


Mesh::~Mesh()
{
}

//PRIVATE
void Mesh::setupMesh()
{
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(Vertex), &verts[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);


	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

	glBindVertexArray(0);
}