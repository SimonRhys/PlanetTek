#include "TerrainBlock.h"



TerrainBlock::TerrainBlock()
{
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	this->inUse = false;
}

void TerrainBlock::create(Shader *shader, std::map<std::string, GLuint> *uniformLocations)
{

	this->shader = shader;
	this->uniformLocations = uniformLocations;
	this->inUse = true;
}

void TerrainBlock::draw(glm::mat4 proj, glm::mat4 view, float radius, int lod)
{
	if (!inUse)
	{
		return;
	}

	glUseProgram(shader->getShaderProgram());

	glUniformMatrix4fv(uniformLocations->at("projection"), 1, GL_FALSE, glm::value_ptr(proj));
	glUniformMatrix4fv(uniformLocations->at("view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(uniformLocations->at("model"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1)));

	glUniform3f(uniformLocations->at("lightPos"), -radius*1.5, radius*1.5, radius*1.5);
	glUniform3f(uniformLocations->at("lightColor"), 1.0f, 0.0f, 0.0f);
	glUniform3f(uniformLocations->at("objectColor"), 1.0f, 1.0f, 1.0f);

	if (lod > 4)
	{
		glUniform3f(uniformLocations->at("lightColor"), 0.0f, 1.0f, 0.0f);
	}
	
	if (lod > 9)
	{
		glUniform3f(uniformLocations->at("lightColor"), 0.0f, 0.0f, 1.0f);
	}

	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glUseProgram(0);
}

void TerrainBlock::generate(glm::vec2 start, glm::vec2 end, glm::vec2 mapSize, float radius, int lod)
{
	vertices.clear();
	indices.clear();

	for (int x = start.x; x < end.x; x += lod)
	{
		glm::vec2 yRange = glm::vec2(start.y, end.y);
		addCol(x, yRange, mapSize, radius, lod);
	}

	//Need to add an extra row at the end
	//to ensure we use the value mapSize.x
	//
	//Note: We can't use <= in the loop above as
	//		with a custom modifier it might never = mapSize.x
	if (end.x == mapSize.x)
	{
		glm::vec2 yRange = glm::vec2(start.y, end.y);
		addCol(end.x, yRange, mapSize, radius, lod);
	}


	glm::vec2 length = glm::ceil((end - start) / glm::vec2(lod, lod));
	if (end.x == mapSize.x)
	{
		length.x += 1;
	}

	if (end.y == mapSize.y)
	{
		length.y += 1;
	}

	for (int i = 0; i < length.x-1; i++) 
	{
		for (int j = 0; j < length.y-1; j++) 
		{
			indices.push_back(i*length.y + j);
			indices.push_back((i + 1)*length.y + j);
			indices.push_back(i*length.y + j + 1);

			indices.push_back(i*length.y + j + 1);
			indices.push_back((i + 1)*length.y + j);
			indices.push_back((i + 1)*length.y + j + 1);
		}
	}

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices.front(), GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (GLvoid*)(1 * sizeof(glm::vec3)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices.front(), GL_DYNAMIC_DRAW);

	glBindVertexArray(0);

}

TerrainBlock::~TerrainBlock()
{
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);
}

//Private
void TerrainBlock::addCol(int xValue, glm::vec2 yRange, glm::vec2 mapSize, float radius, int lod)
{
	int y = yRange.x;
	while (y < yRange.y)
	{
		glm::vec3 coords = mapCartesianToSpherical(glm::vec2(xValue, y), mapSize, radius);
		vertices.push_back(coords);
		vertices.push_back(coords);

		y += lod;

		//We want to perform one last vertex addition as the sphere
		//cannot generate properly unless mapSize.y is included in 
		//the range of vertices
		if (y >= mapSize.y)
		{
			coords = mapCartesianToSpherical(glm::vec2(xValue, mapSize.y), mapSize, radius);
			vertices.push_back(coords);
			vertices.push_back(coords);
		}
	}
}

glm::vec3 TerrainBlock::mapCartesianToSpherical(glm::vec2 coords, glm::vec2 mapSize, float radius)
{
	float thetaDelta = PI / mapSize.x;
	float phiDelta = TWO_PI / mapSize.y;
	float theta = coords.y * thetaDelta;
	float phi = coords.x * phiDelta;

	glm::vec3 coords2;
	coords2.x = radius * glm::sin(theta) * glm::cos(phi);
	coords2.y = radius * glm::sin(theta) * glm::sin(phi);
	coords2.z = radius * glm::cos(theta);
	return coords2;
}