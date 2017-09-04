#pragma once
#include <map>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <SOIL/SOIL.h>
#include "Shader.h"

class TerrainShader
{
public:
	TerrainShader();

	void calcReflection(bool r);
	void calcRefraction(bool r);
	void drawSea(bool b);
	void init(float *w, float *h);
	void loadTexture(std::string filePath, std::string textureName);
	void setCamera(glm::vec3 *camera);
	void setRadius(float r);
	void setSeaLevel(float level);
	void setSunPosition(glm::vec3 pos);
	void setWaveSpeed(float speed);
	void use(glm::mat4 proj, glm::vec3 viewPos, glm::vec2 viewRotation);

	~TerrainShader();

private:

	void createShaderProgram();
	void createFBOs();
	GLuint createTextureAttachment(int width, int height);
	GLuint createDepthTextureAttachment(int width, int height);
	GLuint createDepthBufferAttachment(int width, int height);

	GLuint reflectionFBO;
	GLuint refractionFBO;
	GLuint reflectionTextureID;
	GLuint refractionTextureID;
	GLuint refractionDepthTextureID;
	GLuint reflectionDepthBuffer;

	Shader shader;
	std::map<std::string, GLuint> uniformLocations;
	int renderSea;
	int reflection;
	int refraction;

	glm::vec2 reflectionResolution = glm::vec2(800, 600);
	glm::vec2 refractionResolution = glm::vec2(800, 600);

	glm::vec3 *camera;
	float *windowWidth;
	float *windowHeight;

	float radius;
	float seaLevel;
	float waveSpeed;
	glm::vec3 sunPosition;

	GLuint textures[16];
	int numTexturesLoaded = 0;

};

