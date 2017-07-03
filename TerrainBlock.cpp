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
}

void TerrainBlock::draw(glm::mat4 proj, glm::mat4 view, float radius)
{
	if (!inUse)
	{
		return;
	}

	glUseProgram(shader->getShaderProgram());

	glUniformMatrix4fv(uniformLocations->at("projection"), 1, GL_FALSE, glm::value_ptr(proj));
	glUniformMatrix4fv(uniformLocations->at("view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(uniformLocations->at("model"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1)));

	glUniform3f(uniformLocations->at("lightPos"), -10000*1.5, 10000*1.5, 10000*1.5);
	glUniform3f(uniformLocations->at("lightColor"), 1.0f, 0.0f, 0.0f);
	glUniform3f(uniformLocations->at("objectColor"), 1.0f, 1.0f, 1.0f);

	if (LOD > 4)
	{
		glUniform3f(uniformLocations->at("lightColor"), 0.0f, 1.0f, 0.0f);
	}
	
	if (LOD > 9)
	{
		glUniform3f(uniformLocations->at("lightColor"), 0.0f, 0.0f, 1.0f);
	}

	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glUseProgram(0);
}
void TerrainBlock::generate(glm::vec2 start, glm::vec2 end, Heightmap *heightmap, float radius, int lod)
{
	this->inUse = false;
	this->LOD = lod;
	this->startPoint = start;
	this->endPoint = end;

	vertices.clear();
	indices.clear();

	generateVertices(start, end, heightmap, radius, lod);

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

	this->inUse = true;
}

void TerrainBlock::markUnused()
{
	this->inUse = false;
}

bool TerrainBlock::isUsed()
{
	return this->inUse;
}

glm::vec2 TerrainBlock::getStartPoint()
{
	return this->startPoint;
}


glm::vec2 TerrainBlock::getEndPoint()
{
	return this->endPoint;
}

void TerrainBlock::generateVertices(glm::vec2 start, glm::vec2 end, Heightmap *heightmap, float radius, int lod)
{

	//In case we already have stuff in the vertex buffer
	//we need to be able to offset our indices
	float indexOffset = vertices.size() / 2;

	for (int x = start.x; x < end.x; x += lod)
	{
		int y = start.y;
		while(y < end.y)
		{
			glm::vec3 coords = mapOctohedronToSphere(glm::vec2(x, y), heightmap, radius+heightmap->get(x, y));
			vertices.push_back(coords);
			vertices.push_back(coords);

			y += lod;

			//We want to perform one last vertex addition as the sphere
			//cannot generate properly unless mapSize.y is included in 
			//the range of vertices
			if (y >= end.y)
			{
				glm::vec3 coords = mapOctohedronToSphere(glm::vec2(x, end.y), heightmap, radius + heightmap->get(x, end.y));
				vertices.push_back(coords);
				vertices.push_back(coords);
			}
		}
	}

	//Need to add an extra row at the end
	//to ensure we use the value mapSize.x
	//
	//Note: We can't use <= in the loop above as
	//		with a custom modifier it might never = mapSize.x

	int y = start.y;
	while( y < end.y)
	{
		glm::vec3 coords = mapOctohedronToSphere(glm::vec2(end.x, y), heightmap, radius + heightmap->get(end.x, y));
		vertices.push_back(coords);
		vertices.push_back(coords);
		
		y += lod;

		//We want to perform one last vertex addition as the sphere
		//cannot generate properly unless mapSize.y is included in 
		//the range of vertices
		if (y >= end.y)
		{
			glm::vec3 coords = mapOctohedronToSphere(glm::vec2(end.x, end.y), heightmap, radius + heightmap->get(end.x, end.y));
			vertices.push_back(coords);
			vertices.push_back(coords);
		}
	}



	glm::vec2 length = glm::ceil((end - start) / glm::vec2(lod, lod));
	length.x += 1;
	length.y += 1;

	for (int i = 0; i < length.x - 1; i++)
	{
		for (int j = 0; j < length.y - 1; j++)
		{
			indices.push_back(i*length.y + j + indexOffset);
			indices.push_back((i + 1)*length.y + j + indexOffset);
			indices.push_back(i*length.y + j + 1 + indexOffset);

			indices.push_back((i + 1)*length.y + j + indexOffset);
			indices.push_back((i + 1)*length.y + j + 1 + indexOffset);
			indices.push_back(i*length.y + j + 1 + indexOffset);
		}
	}
}


TerrainBlock::~TerrainBlock()
{
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);
}

//Private
glm::vec3 TerrainBlock::mapCubeToSphere(glm::vec3 coords, float radius)
{
	glm::vec3 coords2;
	coords2 = glm::normalize(coords) * radius;

	return coords2;
}

glm::vec3 TerrainBlock::mapOctohedronToSphere(glm::vec2 coords, Heightmap *heightmap, float radius)
{
	glm::vec3 xVec;
	glm::vec3 yVec;
	glm::vec3 zVec;

	float width = heightmap->getWidth();
	float height = heightmap->getHeight();

	float halfWidth = width / 2;
	float halfHeight = height / 2;

	glm::vec3 coords2;

	if (coords.x < halfWidth)
	{
		xVec = glm::vec3(-1, 0, 0);
	}
	else
	{
		xVec = glm::vec3(1, 0, 0);
	}

	if (coords.y < halfHeight)
	{
		zVec = glm::vec3(0, 0, -1);
	}
	else
	{
		zVec = glm::vec3(0, 0, 1);
	}

	float distToCentre = glm::distance(coords, glm::vec2(halfWidth, halfHeight));
	float distToTopLeft = glm::distance(coords, glm::vec2(0, 0));
	float distToTopRight = glm::distance(coords, glm::vec2(width, 0));
	float distToBotLeft = glm::distance(coords, glm::vec2(0, height));
	float distToBotRight = glm::distance(coords, glm::vec2(width, height));

	bool closestToCentre = true;

	closestToCentre = closestToCentre && distToCentre < distToTopLeft;
	closestToCentre = closestToCentre && distToCentre < distToTopRight;
	closestToCentre = closestToCentre && distToCentre < distToBotLeft;
	closestToCentre = closestToCentre && distToCentre < distToBotRight;

	if (closestToCentre)
	{
		yVec = glm::vec3(0, 1, 0);

		if (xVec == glm::vec3(-1, 0, 0))
		{
			coords2 = lerp(xVec, yVec, coords.x / halfWidth);
		}
		else
		{
			coords2 = lerp(yVec, xVec, (coords.x - halfWidth) / halfWidth);
		}

		if (zVec == glm::vec3(0, 0, -1))
		{
			coords2 += lerp(zVec, yVec, coords.y / halfHeight);
		}
		else
		{
			coords2 += lerp(yVec, zVec, (coords.y - halfHeight) / halfHeight);
		}
	}
	else
	{
		yVec = glm::vec3(0, -1, 0);

		if (xVec == glm::vec3(-1, 0, 0))
		{
			coords2 = lerp(yVec, zVec, coords.x / halfWidth);
		}
		else
		{
			coords2 = lerp(zVec, yVec, (coords.x - halfWidth) / halfWidth);
		}

		if (zVec == glm::vec3(0, 0, -1))
		{
			coords2 += lerp(yVec, xVec, coords.y / halfHeight);
		}
		else
		{
			coords2 += lerp(xVec, yVec, (coords.y - halfHeight) / halfHeight);
		}


	}

	coords2 -= yVec;
	coords2 = glm::normalize(coords2) * radius;

	return coords2;
}

glm::vec3 TerrainBlock::lerp(glm::vec3 v1, glm::vec3 v2, float p)
{
	glm::vec3 dist = v2 - v1;
	dist = dist * p;

	return v1 + dist;
}