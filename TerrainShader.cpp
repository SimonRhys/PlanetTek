#include "TerrainShader.h"



TerrainShader::TerrainShader()
{
}

void TerrainShader::calcReflection(bool b)
{
	reflection = b;
}

void TerrainShader::calcRefraction(bool b)
{
	refraction = b;
}

void TerrainShader::drawSea(bool b)
{
	renderSea = b;
}

void TerrainShader::init(float *w, float *h)
{
	windowWidth = w;
	windowHeight = h;
	createShaderProgram();
	createFBOs();
}

void TerrainShader::loadTexture(std::string filePath, std::string textureName)
{
	int w, h, channels;
	unsigned char *image = SOIL_load_image(filePath.c_str(), &w, &h, &channels, SOIL_LOAD_RGB);
	if (image == 0)
	{
		std::cout << "SOIL failed to load Planet texture!" << std::endl;
		return;
	}
	else
	{
		std::cout << "SOIL loaded Planet texture!" << std::endl;
	}

	glGenTextures(1, &textures[numTexturesLoaded]);

	glBindTexture(GL_TEXTURE_2D, textures[numTexturesLoaded]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glUseProgram(shader.getShaderProgram());
	glUniform1i(glGetUniformLocation(shader.getShaderProgram(), textureName.c_str()), numTexturesLoaded);
	glUseProgram(0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenerateMipmap(GL_TEXTURE_2D);

	numTexturesLoaded++;
	SOIL_free_image_data(image);
}

void TerrainShader::setCamera(glm::vec3 *c)
{
	camera = c;
}

void TerrainShader::setRadius(float r)
{
	radius = r;
}

void TerrainShader::setSeaLevel(float level)
{
	seaLevel = level;
}

void TerrainShader::setSunPosition(glm::vec3 pos)
{
	sunPosition = pos;
}

void TerrainShader::setWaveSpeed(float speed)
{
	waveSpeed = speed;
}

void TerrainShader::use(glm::mat4 proj, glm::vec3 viewPos, glm::vec2 viewRotation)
{
	glm::mat4 view;

	if (reflection)
	{
		float distance = 2.f * (glm::length(viewPos) - seaLevel);
		glm::vec3 dir = glm::normalize(viewPos);

		view = glm::rotate(-viewRotation.x, glm::vec3(1, 0, 0));
		view = view * glm::rotate(viewRotation.y, glm::vec3(0, 1, 0));
		view = view * glm::translate(viewPos - dir*distance);
	}
	else
	{
		view = glm::rotate(viewRotation.x, glm::vec3(1, 0, 0));
		view = view * glm::rotate(viewRotation.y, glm::vec3(0, 1, 0));
		view = view * glm::translate(viewPos);
	}

	glUseProgram(shader.getShaderProgram());

	glUniformMatrix4fv(uniformLocations.at("projection"), 1, GL_FALSE, glm::value_ptr(proj));
	glUniformMatrix4fv(uniformLocations.at("view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(uniformLocations.at("model"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1)));

	glUniform3f(uniformLocations.at("lightPos"), sunPosition.x, sunPosition.y, sunPosition.z);
	glUniform3f(uniformLocations.at("lightColor"), 1.0f, 1.0f, 1.0f);
	glUniform3f(uniformLocations.at("eyePos"), -camera->x, -camera->y, -camera->z);
	glUniform3f(uniformLocations.at("skyColour"), 0.298, 0.5, 0.5);

	glUniform1f(uniformLocations.at("seaLevel"), seaLevel);
	glUniform1f(uniformLocations.at("waveSpeed"), waveSpeed);

	glUniform1i(uniformLocations.at("renderSea"), renderSea);
	glUniform1i(uniformLocations.at("reflection"), reflection);
	glUniform1i(uniformLocations.at("refraction"), refraction);

	for (int i = 0; i < numTexturesLoaded; i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, textures[i]);
	}

	if (reflection)
	{
		glEnable(GL_CLIP_DISTANCE0);
		glBindFramebuffer(GL_FRAMEBUFFER, reflectionFBO);
		glViewport(0, 0, reflectionResolution.x, reflectionResolution.y);
	}
	else if (refraction)
	{
		glEnable(GL_CLIP_DISTANCE0);
		glBindFramebuffer(GL_FRAMEBUFFER, refractionFBO);
		glViewport(0, 0, refractionResolution.x, refractionResolution.y);
	}
	else
	{
		glDisable(GL_CLIP_DISTANCE0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, *windowWidth, *windowHeight);
	}
}



TerrainShader::~TerrainShader()
{
	glDeleteTextures(numTexturesLoaded, textures);
}

//PRIVATE
void TerrainShader::createShaderProgram()
{
	shader.createShader("planetVert.sh", GL_VERTEX_SHADER);
	shader.createShader("planetFrag.sh", GL_FRAGMENT_SHADER);
	shader.createShader("planetGeo.sh", GL_GEOMETRY_SHADER);
	shader.createProgram();

	uniformLocations["projection"] = glGetUniformLocation(shader.getShaderProgram(), "projection");
	uniformLocations["view"] = glGetUniformLocation(shader.getShaderProgram(), "view");
	uniformLocations["model"] = glGetUniformLocation(shader.getShaderProgram(), "model");
	uniformLocations["lightPos"] = glGetUniformLocation(shader.getShaderProgram(), "lightPos");
	uniformLocations["lightColor"] = glGetUniformLocation(shader.getShaderProgram(), "lightColor");
	uniformLocations["eyePos"] = glGetUniformLocation(shader.getShaderProgram(), "eyePos");
	uniformLocations["seaLevel"] = glGetUniformLocation(shader.getShaderProgram(), "seaLevel");
	uniformLocations["waveSpeed"] = glGetUniformLocation(shader.getShaderProgram(), "waveSpeed");
	uniformLocations["renderSea"] = glGetUniformLocation(shader.getShaderProgram(), "renderSea");
	uniformLocations["skyColour"] = glGetUniformLocation(shader.getShaderProgram(), "skyColour");
	uniformLocations["reflectionTexture"] = glGetUniformLocation(shader.getShaderProgram(), "reflectionTexture");
	uniformLocations["refractionTexture"] = glGetUniformLocation(shader.getShaderProgram(), "refractionTexture");
	uniformLocations["reflection"] = glGetUniformLocation(shader.getShaderProgram(), "reflection");
	uniformLocations["refraction"] = glGetUniformLocation(shader.getShaderProgram(), "refraction");
}

void TerrainShader::createFBOs()
{
	glGenFramebuffers(1, &reflectionFBO);
	glGenFramebuffers(1, &refractionFBO);

	glBindFramebuffer(GL_FRAMEBUFFER, reflectionFBO);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	reflectionTextureID = createTextureAttachment(reflectionResolution.x, reflectionResolution.y);
	reflectionDepthBuffer = createDepthBufferAttachment(reflectionResolution.x, reflectionResolution.y);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "REFLECTION FRAMEBUFFER NOT COMPLETE" << std::endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, refractionFBO);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	refractionTextureID = createTextureAttachment(refractionResolution.x, refractionResolution.y);
	refractionDepthTextureID = createDepthTextureAttachment(refractionResolution.x, refractionResolution.y);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "REFRACTION FRAMEBUFFER NOT COMPLETE" << std::endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glUseProgram(shader.getShaderProgram());
	glUniform1i(glGetUniformLocation(shader.getShaderProgram(), "reflectionTexture"), numTexturesLoaded);
	textures[numTexturesLoaded++] = reflectionTextureID;
	glUniform1i(glGetUniformLocation(shader.getShaderProgram(), "refractionTexture"), numTexturesLoaded);
	textures[numTexturesLoaded++] = refractionTextureID;
	glUseProgram(0);
}


GLuint TerrainShader::createTextureAttachment(int width, int height)
{
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture, 0);

	return texture;
}

GLuint TerrainShader::createDepthTextureAttachment(int width, int height)
{
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texture, 0);

	return texture;
}

GLuint TerrainShader::createDepthBufferAttachment(int width, int height)
{
	GLuint depthBuffer;
	glGenRenderbuffers(1, &depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

	return depthBuffer;
}

