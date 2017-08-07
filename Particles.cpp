#include "Particles.h"



Particles::Particles()
{
}

void Particles::draw()
{
	if (!initialised)
	{
		return;
	}

	glUseProgram(shader.getShaderProgram());

	/*glm::mat4 proj = glm::mat4(1); //glm::ortho(0, 800, 0, 600);
	glm::mat4 view = glm::mat4(1);

	glUniformMatrix4fv(uniformLocations.at("projection"), 1, GL_FALSE, glm::value_ptr(proj));
	glUniformMatrix4fv(uniformLocations.at("view"), 1, GL_FALSE, glm::value_ptr(view));*/
	glUniformMatrix4fv(uniformLocations.at("model"), 1, GL_FALSE, glm::value_ptr(glm::scale(glm::vec3(0.05))));

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, particlePositionBuffer);
	glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * sizeof(glm::vec3), NULL, GL_STREAM_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, particleCount * sizeof(glm::vec3), positionData);

	glBindBuffer(GL_ARRAY_BUFFER, particleColourBuffer);
	glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * 3 * sizeof(GLubyte), NULL, GL_STREAM_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, particleCount * sizeof(GLubyte) * 3, colourData);

	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, particleCount);

	glUseProgram(0);
}

void Particles::init()
{
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vertexBuffer);
	glGenBuffers(1, &particlePositionBuffer);
	glGenBuffers(1, &particleColourBuffer);

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, particlePositionBuffer);
	glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * sizeof(glm::vec3), NULL, GL_STREAM_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, particleColourBuffer);
	glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * 3 * sizeof(GLubyte), NULL, GL_STREAM_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);
	
	glBindBuffer(GL_ARRAY_BUFFER, particlePositionBuffer);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, particleColourBuffer);
	glVertexAttribPointer(2, 3, GL_UNSIGNED_BYTE, GL_TRUE, 3 * sizeof(GLubyte), (void*)0);
	glEnableVertexAttribArray(2);

	glVertexAttribDivisor(0, 0);
	glVertexAttribDivisor(1, 1); 
	glVertexAttribDivisor(2, 1); 

	glBindVertexArray(0);

	particleContainer = new Particle[MAX_PARTICLES];

	positionData = new glm::vec3[MAX_PARTICLES];
	colourData = new unsigned char[MAX_PARTICLES * 3];

	float w = 0.15;
	float h = 0;

	for (int i = 0; i < MAX_PARTICLES; i++)
	{

		Particle& p = particleContainer[i];

		p.life = 5;
		p.pos = glm::vec3(-20+w, 10+h, 0);
		p.r = 150;
		p.g = 200;
		p.b = 240;

		w += 1;

		if (i % 100 == 0 && i != 0)
		{
			h += 1;
			w = 0;
		}
	}

	createShaderProgram();

	initialised = true;
}

void Particles::update(float dt)
{
	if (!initialised)
	{
		return;
	}

	particleCount = 0;
	for (int i = 0; i< MAX_PARTICLES; i++) 
	{

		Particle& p = particleContainer[i];

		if (p.life > 0.0f) 
		{

			p.life -= dt;
			if (p.life > 0.0f) 
			{

				p.speed = glm::vec3(0.0f, -9.81f, 0.0f) * (float)dt * 1500.f;
				p.pos += p.speed * (float)dt;

				positionData[particleCount] = p.pos;

				colourData[3 * particleCount + 0] = p.r;
				colourData[3 * particleCount + 1] = p.g;
				colourData[3 * particleCount + 2] = p.b;

				particleCount++;

			}
		}
	}
}

Particles::~Particles()
{
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vertexBuffer);
}

//PRIVATE
void Particles::createShaderProgram()
{
	shader.createShader("particleVert.sh", GL_VERTEX_SHADER);
	shader.createShader("particleFrag.sh", GL_FRAGMENT_SHADER);
	shader.createProgram();

	uniformLocations["projection"] = glGetUniformLocation(shader.getShaderProgram(), "projection");
	uniformLocations["view"] = glGetUniformLocation(shader.getShaderProgram(), "view");
	uniformLocations["model"] = glGetUniformLocation(shader.getShaderProgram(), "model");
}
