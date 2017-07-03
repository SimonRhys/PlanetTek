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
void TerrainBlock::generate(glm::vec3 start, glm::vec3 end, Heightmap *heightmap, float radius, int lod)
{
	this->inUse = false;
	this->LOD = lod;
	this->startPoints.push_back(start);
	this->endPoints.push_back(end);

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


void TerrainBlock::generate(std::vector<glm::vec3> start, std::vector<glm::vec3> end, Heightmap *heightmap, float radius, int lod)
{
	this->inUse = false;
	this->LOD = lod;
	this->startPoints = start;
	this->endPoints = end;

	vertices.clear();
	indices.clear();

	for (int i = 0; i < start.size(); i++)
	{
		generateVertices(start[i], end[i], heightmap, radius, lod);
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

	this->inUse = true;
}

void TerrainBlock::markUnused()
{
	this->inUse = false;
}

bool TerrainBlock::isUsed()
{
	return inUse;
}

std::vector<glm::vec3> TerrainBlock::getStartPoints()
{
	return this->startPoints;
}


std::vector<glm::vec3> TerrainBlock::getEndPoints()
{
	return this->endPoints;
}

void TerrainBlock::generateVertices(glm::vec3 start, glm::vec3 end, Heightmap *heightmap, float radius, int lod)
{
	//We have to change our loop starts and ends as we could
	//have a loop of of 32 -> -32 which is valid but would fail
	//the generic loop structure
	glm::vec2 loopStart; 
	glm::vec2 loopEnd;
	glm::vec3 outerLoopModifier;
	glm::vec3 innerLoopModifier;
	glm::vec3 startModifier;

	if (start.x == end.x)
	{
		loopStart = glm::vec2(start.z, start.y);
		loopEnd = glm::vec2(end.z, end.y);

		loopStart.x = glm::min(start.z, end.z);
		loopStart.y = glm::min(start.y, end.y);

		loopEnd.x = glm::max(start.z, end.z);
		loopEnd.y = glm::max(start.y, end.y);

		outerLoopModifier = glm::vec3(0, 0, 1);
		innerLoopModifier = glm::vec3(0, 1, 0);
		startModifier = glm::vec3(1, 0, 0);
	}
	else if (start.y == end.y)
	{
		loopStart = glm::vec2(start.x, start.z);
		loopEnd = glm::vec2(end.x, end.z);

		loopStart.x = glm::min(start.x, end.x);
		loopStart.y = glm::min(start.z, end.z);

		loopEnd.x = glm::max(start.x, end.x);
		loopEnd.y = glm::max(start.z, end.z);

		outerLoopModifier = glm::vec3(1, 0, 0);
		innerLoopModifier = glm::vec3(0, 0, 1);
		startModifier = glm::vec3(0, 1, 0);

	}
	else if (start.z == end.z)
	{
		loopStart = glm::vec2(start.x, start.y);
		loopEnd = glm::vec2(end.x, end.y);

		loopStart.x = glm::min(start.x, end.x);
		loopStart.y = glm::min(start.y, end.y);

		loopEnd.x = glm::max(start.x, end.x);
		loopEnd.y = glm::max(start.y, end.y);

		outerLoopModifier = glm::vec3(1, 0, 0);
		innerLoopModifier = glm::vec3(0, 1, 0);
		startModifier = glm::vec3(0, 0, 1);

	}

	//In case we already have stuff in the vertex buffer
	//we need to be able to offset our indices
	float indexOffset = vertices.size() / 2;

	for (int x = loopStart.x; x < loopEnd.x; x += lod)
	{
		int y = loopStart.y;
		while(y < loopEnd.y)
		{
			glm::vec3 sample(0, 0, 0);
			sample += startModifier*start;
			sample += outerLoopModifier*(float)x;
			sample += innerLoopModifier*(float)y;
			glm::vec3 coords = mapCubeToSphere(sample, radius+heightmap->get(sample));
			vertices.push_back(coords);
			vertices.push_back(coords);

			y += lod;

			//We want to perform one last vertex addition as the sphere
			//cannot generate properly unless mapSize.y is included in 
			//the range of vertices
			if (y >= loopEnd.y)
			{
				sample = glm::vec3(0, 0, 0);
				sample += startModifier*start;
				sample += outerLoopModifier*(float)x;
				sample += innerLoopModifier*(float)loopEnd.y;
				coords = mapCubeToSphere(sample, radius+heightmap->get(sample));
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

	int y = loopStart.y;
	while( y < loopEnd.y)
	{
		glm::vec3 sample(0, 0, 0);
		sample += startModifier*start;
		sample += outerLoopModifier*loopEnd.x;
		sample += innerLoopModifier*(float)y;
		glm::vec3 coords = mapCubeToSphere(sample, radius+heightmap->get(sample));
		vertices.push_back(coords);
		vertices.push_back(coords);
		
		y += lod;

		//We want to perform one last vertex addition as the sphere
		//cannot generate properly unless mapSize.y is included in 
		//the range of vertices
		if (y >= loopEnd.y)
		{
			sample = glm::vec3(0, 0, 0);
			sample += startModifier*start;
			sample += outerLoopModifier*loopEnd.x;
			sample += innerLoopModifier*(float)loopEnd.y;
			coords = mapCubeToSphere(sample, radius+heightmap->get(sample));
			vertices.push_back(coords);
			vertices.push_back(coords);
		}
	}



	glm::vec2 length = glm::ceil((loopEnd - loopStart) / glm::vec2(lod, lod));
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