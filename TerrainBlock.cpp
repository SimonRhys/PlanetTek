#include "TerrainBlock.h"

void TerrainBlock::createVBO()
{
	glBindVertexArray(vaoLow);
	glBindBuffer(GL_ARRAY_BUFFER, vboLow);
	glBufferData(GL_ARRAY_BUFFER, verticesLow.size() * sizeof(glm::vec3), &verticesLow.front(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(glm::vec3), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(glm::vec3), (GLvoid*)(1 * sizeof(glm::vec3)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(glm::vec3), (GLvoid*)(2 * sizeof(glm::vec3)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(vaoMed);
	glBindBuffer(GL_ARRAY_BUFFER, vboMed);
	glBufferData(GL_ARRAY_BUFFER, verticesMed.size() * sizeof(glm::vec3), &verticesMed.front(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(glm::vec3), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(glm::vec3), (GLvoid*)(1 * sizeof(glm::vec3)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(glm::vec3), (GLvoid*)(2 * sizeof(glm::vec3)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(vaoHigh);
	glBindBuffer(GL_ARRAY_BUFFER, vboHigh);
	glBufferData(GL_ARRAY_BUFFER, verticesHigh.size() * sizeof(glm::vec3), &verticesHigh.front(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(glm::vec3), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(glm::vec3), (GLvoid*)(1 * sizeof(glm::vec3)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(glm::vec3), (GLvoid*)(2 * sizeof(glm::vec3)));
	glEnableVertexAttribArray(2);


	glBindVertexArray(0);
}

TerrainBlock::TerrainBlock()
{
	glGenVertexArrays(1, &vaoLow);
	glGenBuffers(1, &vboLow);

	glGenVertexArrays(1, &vaoMed);
	glGenBuffers(1, &vboMed);

	glGenVertexArrays(1, &vaoHigh);
	glGenBuffers(1, &vboHigh);

	this->inUse = false;
}

void TerrainBlock::draw(glm::mat4 proj, glm::mat4 view, glm::vec3 cameraPos)
{
	if (!inUse)
	{
		return;
	}

	int lod;

	glm::vec3 normCam = glm::normalize(cameraPos) * radius;
	float distanceC1 = calcGreatCircleDistance(normCam, c1, radius);
	float distanceC2 = calcGreatCircleDistance(normCam, c2, radius);
	float distanceC3 = calcGreatCircleDistance(normCam, c3, radius);
	float distanceC4 = calcGreatCircleDistance(normCam, c4, radius);
	float sizeC1 = calcGreatCircleDistance(c1, centrePoint, radius);
	float sizeC2 = calcGreatCircleDistance(c2, centrePoint, radius);
	float sizeC3 = calcGreatCircleDistance(c3, centrePoint, radius);
	float sizeC4 = calcGreatCircleDistance(c4, centrePoint, radius);

	bool d = distanceC1 < sizeC1 / 2;
	d = d || distanceC2 < sizeC2 / 2;
	d = d || distanceC3 < sizeC3 / 2;
	d = d || distanceC4 < sizeC4 / 2;

	if (d)
	{
		lod = HIGH_QUALITY;
	}
	else
	{
		lod = LOW_QUALITY;
	}

	if (lod == LOW_QUALITY)
	{
		glBindVertexArray(vaoLow);
		glDrawArrays(GL_TRIANGLES, 0, verticesLow.size() / 3);
		glBindVertexArray(0);
	}
	else if (lod == MED_QUALITY)
	{
		glBindVertexArray(vaoMed);
		glDrawArrays(GL_TRIANGLES, 0, verticesMed.size() / 3);
		glBindVertexArray(0);
	}
	else if (lod == HIGH_QUALITY)
	{
		glBindVertexArray(vaoHigh);
		glDrawArrays(GL_TRIANGLES, 0, verticesHigh.size() / 3);
		glBindVertexArray(0);
	}

}

void TerrainBlock::generate(glm::vec2 start, glm::vec2 end, Heightmap *heightmap, float radius)
{
	this->inUse = false;
	this->startPoint = start;
	this->endPoint = end;
	this->radius = radius;

	verticesLow.clear();
	verticesMed.clear();
	verticesHigh.clear();

	generateVertices(start, end, heightmap, radius);

	this->inUse = true;
}

void TerrainBlock::markUnused()
{
	this->inUse = false;
}

void TerrainBlock::setWindowSize(GLfloat *width, GLfloat *height)
{
}


void TerrainBlock::update(float dt)
{
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

glm::vec3 TerrainBlock::getCentrePoint()
{
	return this->centrePoint;
}

void TerrainBlock::generateVertices(glm::vec2 start, glm::vec2 end, Heightmap *heightmap, float radius)
{
	std::vector<glm::vec3> vertexList;

	for (int x = start.x; x < end.x; x++)
	{
		int y = start.y;
		while(y < end.y)
		{
			glm::vec3 coords = mapOctohedronToSphere(glm::vec2(x, y), heightmap, radius+heightmap->get(x, y));
			vertexList.push_back(coords);
			vertexList.push_back(coords);

			y++;

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
		
		y++;

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

	createVertexLists(start, end, &vertexList, HIGH_QUALITY);
	createVertexLists(start, end, &vertexList, MED_QUALITY);
	createVertexLists(start, end, &vertexList, LOW_QUALITY);

	glm::vec2 centreCoord = glm::vec2((end - start) / 2.f);
	centrePoint = mapOctohedronToSphere(centreCoord, heightmap, radius+heightmap->get(centreCoord.x, centreCoord.y));

	c1 = mapOctohedronToSphere(start, heightmap, radius);
	c2 = mapOctohedronToSphere(glm::vec2(endPoint.x, startPoint.y), heightmap, radius);
	c3 = mapOctohedronToSphere(glm::vec2(startPoint.x, endPoint.y), heightmap, radius);
	c4 = mapOctohedronToSphere(end, heightmap, radius);
}

float TerrainBlock::calcGreatCircleDistance(float phi1, float lambda1, float phi2, float lambda2, float radius)
{
	float deltaPhi = glm::abs(phi1 - phi2);
	float deltaLambda = glm::abs(lambda1 - lambda2);
	float centralAngle = glm::sin(deltaPhi/2) * glm::sin(deltaPhi/2);
	centralAngle += glm::cos(phi1) * glm::cos(phi2) * glm::sin(deltaLambda/2) * glm::sin(deltaLambda/2);
	centralAngle = glm::sqrt(centralAngle);
	centralAngle = 2 * glm::asin(centralAngle);

	return radius*centralAngle;
}

float TerrainBlock::calcGreatCircleDistance(glm::vec3 p1, glm::vec3 p2, float radius)
{
	float lambda1 = glm::acos(p1.z / radius);
	float lambda2 = glm::acos(p2.z / radius);

	float phi1; 
	float phi2;

	if (p1.x != 0)
	{
		phi1 = glm::acos(p1.x / (radius * glm::sin(lambda1)));
	}
	else
	{
		phi1 = glm::asin(p1.y / (radius * glm::sin(lambda1)));
	}

	if (p2.x != 0)
	{
		phi2 = glm::acos(p2.x / (radius * glm::sin(lambda2)));
	}
	else
	{
		phi2 = glm::asin(p2.y / (radius * glm::sin(lambda2)));
	}

	return calcGreatCircleDistance(phi1, lambda1, phi2, lambda2, radius);
}

glm::vec2 TerrainBlock::calcAngles(glm::vec3 p)
{
	float lambda = glm::acos(p.z / radius);
	float phi;

	if (p.x != 0)
	{
		phi = glm::acos(p.x / (radius * glm::sin(lambda)));
	}
	else
	{
		phi = glm::asin(p.y / (radius * glm::sin(lambda)));
	}

	return glm::vec2(phi, lambda);
}


void TerrainBlock::createVertexLists(glm::vec2 start, glm::vec2 end, std::vector<glm::vec3> *vertexList, int lod)
{
	glm::vec2 length = end - start;
	length.x += 1;
	length.y += 1;
	
	std::vector<glm::vec3> *vertices = nullptr;

	if (lod == LOW_QUALITY)
	{
		vertices = &verticesLow;
	}
	else if (lod == MED_QUALITY)
	{
		vertices = &verticesMed;
	}
	else if (lod == HIGH_QUALITY)
	{
		vertices = &verticesHigh;
	}

	glm::vec3 texCoord(0);
	glm::vec3 texCoordSize(0.2, 0.2, 0);

	for (int i = 0; i < length.x - 1; i+=lod)
	{
		for (int j = 0; j < length.y - 1; j+=lod)
		{
			int vec1 = i*length.y + j;
			int vec2 = vec1 + lod;
			int vec3 = (i + lod)*length.y + j;
			int vec4 = vec3 + lod;

			//First Triangle
			vertices->push_back(vertexList->at(2 * vec1));
			vertices->push_back(vertexList->at(2 * vec1 + 1));
			vertices->push_back(texCoord);

			vertices->push_back(vertexList->at(2 * vec3));
			vertices->push_back(vertexList->at(2 * vec3 + 1));
			vertices->push_back(glm::vec3(texCoord.x, texCoord.y + texCoordSize.y, texCoord.z));

			vertices->push_back(vertexList->at(2 * vec2));
			vertices->push_back(vertexList->at(2 * vec2 + 1));
			vertices->push_back(glm::vec3(texCoord.x + texCoordSize.x, texCoord.y, texCoord.z));

			//Second Triangle
			vertices->push_back(vertexList->at(2 * vec2));
			vertices->push_back(vertexList->at(2 * vec2 + 1));
			vertices->push_back(glm::vec3(texCoord.x + texCoordSize.x, texCoord.y, texCoord.z));

			vertices->push_back(vertexList->at(2 * vec3));
			vertices->push_back(vertexList->at(2 * vec3 + 1));
			vertices->push_back(glm::vec3(texCoord.x, texCoord.y + texCoordSize.y, texCoord.z));

			vertices->push_back(vertexList->at(2 * vec4));
			vertices->push_back(vertexList->at(2 * vec4 + 1));
			vertices->push_back(texCoord+texCoordSize);

			texCoord.y += texCoordSize.y;

			if (texCoord.y + texCoordSize.y > 1)
			{
				texCoord.y = 0;
			}

		}

		texCoord.x += texCoordSize.x;

		if (texCoord.x + texCoordSize.x > 1)
		{
			texCoord.x = 0;
		}
	}
}

bool TerrainBlock::insideBlock(glm::vec3 point, float radius)
{
	/*float xDistance = calcGreatCircleDistance(c1, c2, radius);
	float yDistance = calcGreatCircleDistance(c1, c3, radius);

	float pToC1 = calcGreatCircleDistance(point, c1, radius);
	float pToC2 = calcGreatCircleDistance(point, c2, radius);
	float pToC3 = calcGreatCircleDistance(point, c3, radius);
	float pToC4 = calcGreatCircleDistance(point, c4, radius);

	inside = inside && pToC1 < xDistance && pToC1 < yDistance;
	inside = inside && pToC2 < xDistance && pToC2 < yDistance;
	inside = inside && pToC3 < xDistance && pToC3 < yDistance;
	inside = inside && pToC4 < xDistance && pToC4 < yDistance;*/

	glm::vec2 c1Angle = calcAngles(c1);
	glm::vec2 c4Angle = calcAngles(c4);
	glm::vec2 pointAngle = calcAngles(point);

	float minPhi = glm::min(c1Angle.x, c4Angle.x);
	float maxPhi = glm::max(c1Angle.x, c4Angle.x);
	float minLambda = glm::min(c1Angle.y, c4Angle.y);
	float maxLambda = glm::max(c1Angle.y, c4Angle.y);

	bool inside = true;
	inside = inside && minPhi <= pointAngle.x && pointAngle.x <= maxPhi;
	inside = inside && minLambda <= pointAngle.y && pointAngle.y <= maxLambda;

	return inside;
}


TerrainBlock::~TerrainBlock()
{
	glDeleteVertexArrays(1, &vaoLow);
	glDeleteBuffers(1, &vboLow);

	glDeleteVertexArrays(1, &vaoMed);
	glDeleteBuffers(1, &vboMed);

	glDeleteVertexArrays(1, &vaoHigh);
	glDeleteBuffers(1, &vboHigh);
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