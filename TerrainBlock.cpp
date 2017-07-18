#include "TerrainBlock.h"



TerrainBlock::TerrainBlock()
{
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	this->inUse = false;
}

void TerrainBlock::draw(glm::mat4 proj, glm::mat4 view, float radius)
{
	if (!inUse)
	{
		return;
	}

	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 3);
	glBindVertexArray(0);
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

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(glm::vec3), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(glm::vec3), (GLvoid*)(1 * sizeof(glm::vec3)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(glm::vec3), (GLvoid*)(2 * sizeof(glm::vec3)));
	glEnableVertexAttribArray(2);


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
	std::vector<glm::vec3> vertexList;

	for (int x = start.x; x < end.x; x += lod)
	{
		int y = start.y;
		while(y < end.y)
		{
			glm::vec3 coords = mapOctohedronToSphere(glm::vec2(x, y), heightmap, radius+heightmap->get(x, y));
			vertexList.push_back(coords);
			vertexList.push_back(coords);

			y += lod;

			//We want to perform one last vertex addition as the sphere
			//cannot generate properly unless mapSize.y is included in 
			//the range of vertices
			if (y >= end.y)
			{
				glm::vec3 coords = mapOctohedronToSphere(glm::vec2(x, end.y), heightmap, radius + heightmap->get(x, end.y));
				vertexList.push_back(coords);
				vertexList.push_back(coords);
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
		vertexList.push_back(coords);
		vertexList.push_back(coords);
		
		y += lod;

		//We want to perform one last vertex addition as the sphere
		//cannot generate properly unless mapSize.y is included in 
		//the range of vertices
		if (y >= end.y)
		{
			glm::vec3 coords = mapOctohedronToSphere(glm::vec2(end.x, end.y), heightmap, radius + heightmap->get(end.x, end.y));
			vertexList.push_back(coords);
			vertexList.push_back(coords);
		}
	}



	glm::vec2 length = glm::ceil((end - start) / glm::vec2(lod, lod));
	length.x += 1;
	length.y += 1;

	for (int i = 0; i < length.x - 1; i++)
	{
		for (int j = 0; j < length.y - 1; j++)
		{
			int vec1 = i*length.y + j;
			int vec2 = vec1 + 1;
			int vec3 = (i + 1)*length.y + j;
			int vec4 = vec3 + 1;
			
			float vecLength = glm::length(vertexList[2 * vec1]);
			vecLength = glm::max(vecLength, glm::length(vertexList[2 * vec2]));
			vecLength = glm::max(vecLength, glm::length(vertexList[2 * vec3]));
			vecLength = glm::max(vecLength, glm::length(vertexList[2 * vec4]));

			float slopeWeight = 0;

			if (vecLength >= radius*1.2)
			{
				vecLength -= radius*1.2;
				vecLength = vecLength / (radius*1.3 - radius*1.2);
				slopeWeight = glm::min(1.f, vecLength);
			}


			//First Triangle
			vertices.push_back(vertexList[2 * vec1]);
			vertices.push_back(vertexList[2 * vec1 + 1]);
			vertices.push_back(glm::vec3(0, 0, slopeWeight));

			vertices.push_back(vertexList[2 * vec3]);
			vertices.push_back(vertexList[2 * vec3 + 1]);
			vertices.push_back(glm::vec3(0, 1, slopeWeight));

			vertices.push_back(vertexList[2 * vec2]);
			vertices.push_back(vertexList[2 * vec2 + 1]);
			vertices.push_back(glm::vec3(1, 0, slopeWeight));

			//Second Triangle
			vertices.push_back(vertexList[2 * vec2]);
			vertices.push_back(vertexList[2 * vec2 + 1]);
			vertices.push_back(glm::vec3(1, 0, slopeWeight));

			vertices.push_back(vertexList[2 * vec3]);
			vertices.push_back(vertexList[2 * vec3 + 1]);
			vertices.push_back(glm::vec3(0, 1, slopeWeight));

			vertices.push_back(vertexList[2 * vec4]);
			vertices.push_back(vertexList[2 * vec4 + 1]);
			vertices.push_back(glm::vec3(1, 1, slopeWeight));

		}
	}
}


TerrainBlock::~TerrainBlock()
{
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
}

//Private
glm::vec3 TerrainBlock::mapOctohedronToSphere(glm::vec2 coords, Heightmap *heightmap, float radius)
{
	glm::vec3 xVec;
	glm::vec3 yVec;
	glm::vec3 zVec;

	float width = heightmap->getWidth();
	float height = heightmap->getHeight();

	float halfWidth = width / 2;
	float halfHeight = height / 2;

	bool inBounds = false;
	while (!inBounds)
	{
		if (coords.x > width)
		{
			coords.x = 2 * width - coords.x;
			coords.y = height - coords.y;
		}
		else if (coords.x < 0)
		{
			coords.x = -coords.x;
			coords.y = height - coords.y;
		}

		if (coords.y > height)
		{
			coords.x = width - coords.x;
			coords.y = 2 * height - coords.y;
		}
		else if (coords.y < 0)
		{
			coords.x = width - coords.x;
			coords.y = -coords.y;
		}

		inBounds = coords.x <= width && coords.x >= 0;
		inBounds = inBounds && coords.y <= height && coords.y >= 0;
	}

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
	//coords2 = coords2 * 6400.f;
	coords2 = glm::normalize(coords2) * radius;

	return coords2;
}

glm::vec3 TerrainBlock::lerp(glm::vec3 v1, glm::vec3 v2, float p)
{
	glm::vec3 dist = v2 - v1;
	dist = dist * p;

	return v1 + dist;
}

glm::vec2 TerrainBlock::rotate(glm::vec2 v, float a)
{
	glm::vec2 temp;
	temp.x = glm::cos(a) * v.x - glm::sin(a) * v.y;
	temp.y = glm::cos(a) * v.x + glm::sin(a) * v.y;

	return temp;
}