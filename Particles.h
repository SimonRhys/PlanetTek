#pragma once

#include <map>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"

class Particles
{
public:
	Particles();
	~Particles();

	void draw();
	void init(GLfloat *width, GLfloat *height);
	void update(float dt);

private:
	void createShaderProgram();
	void createParticles(int n);

	Shader shader;
	std::map<std::string, GLuint> uniformLocations;


	struct Particle {
		glm::vec3 pos, speed;
		unsigned char r, g, b;
		float life;
	};

	GLuint vao;
	GLuint vertexBuffer;
	GLuint particlePositionBuffer;
	GLuint particleColourBuffer;

	GLfloat vertices[12] = {
		-1.f, -1.f, 0.0f,
		1.f, -1.f, 0.0f,
		-1.f, 1.f, 0.0f,
		1.f, 1.f, 0.0f,
	};
	
	const static int MAX_PARTICLES = 10000;
	Particle *particleContainer;

	glm::vec3 *positionData;
	unsigned char *colourData;

	int particleCount = 0;
	int lastUsedParticle = 0;

	bool initialised = false;
	GLfloat *windowWidth;
	GLfloat *windowHeight;

};

