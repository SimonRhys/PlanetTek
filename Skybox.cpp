#include "Skybox.h"

Skybox::Skybox()
{
	shader.createShader("skyboxVert.sh", GL_VERTEX_SHADER);
	shader.createShader("skyboxFrag.sh", GL_FRAGMENT_SHADER);
	shader.createProgram();

	UNIFORM_LOCATIONS["projection"] = glGetUniformLocation(shader.getShaderProgram(), "projection");
	UNIFORM_LOCATIONS["view"] = glGetUniformLocation(shader.getShaderProgram(), "view");
	UNIFORM_LOCATIONS["model"] = glGetUniformLocation(shader.getShaderProgram(), "model");
	UNIFORM_LOCATIONS["skyboxDay"] = glGetUniformLocation(shader.getShaderProgram(), "skyboxDay");
	UNIFORM_LOCATIONS["skyboxNight"] = glGetUniformLocation(shader.getShaderProgram(), "skyboxNight");
	UNIFORM_LOCATIONS["fogColour"] = glGetUniformLocation(shader.getShaderProgram(), "fogColour");
	UNIFORM_LOCATIONS["dayTime"] = glGetUniformLocation(shader.getShaderProgram(), "dayTime");
	UNIFORM_LOCATIONS["blendFactor"] = glGetUniformLocation(shader.getShaderProgram(), "blendFactor");

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	glBindVertexArray(0);
}

void Skybox::draw(glm::mat4 projection, glm::mat4 view)
{
	glDepthFunc(GL_LEQUAL);
	glUseProgram(shader.getShaderProgram());

	if (dayTexture >= 0)
	{
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(UNIFORM_LOCATIONS["skyboxDay"], 0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, dayTexture);
	}

	if (nightTexture >= 0)
	{
		glActiveTexture(GL_TEXTURE1);
		glUniform1i(UNIFORM_LOCATIONS["skyboxNight"], 1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, nightTexture);
	}

	view = glm::mat4(glm::mat3(view));
	float currRotation = glm::cos(rotation * glm::two_pi<float>() / rotationSpeed);
	glm::mat4 model = glm::rotate(currRotation, glm::vec3(0, 1, 0));
	glUniformMatrix4fv(UNIFORM_LOCATIONS["projection"], 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(UNIFORM_LOCATIONS["view"], 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(UNIFORM_LOCATIONS["model"], 1, GL_FALSE, glm::value_ptr(model));
	glUniform3f(UNIFORM_LOCATIONS["fogColour"], 0.298, 0.5, 0.5);

	if (dayTime)
	{
		glUniform1i(UNIFORM_LOCATIONS["dayTime"], 1);
	}
	else
	{
		glUniform1i(UNIFORM_LOCATIONS["dayTime"], 0);
		float blendFactor = glm::sin(time*glm::pi<float>() / nightLength);
		glUniform1f(UNIFORM_LOCATIONS["blendFactor"], blendFactor);

	}


	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthFunc(GL_LESS);
}

void Skybox::setDayLength(float dayLength)
{
	this->dayLength = dayLength;
}

void Skybox::setNightLength(float nightLength)
{
	this->nightLength = nightLength;

}

void Skybox::update(float dt)
{
	time += dt;

	if (time > dayLength && dayTime)
	{
		time = 0;
		dayTime = false;
	}

	if (time > nightLength && !dayTime)
	{
		time = 0;
		dayTime = true;
	}

	rotation += dt;
	
}

bool Skybox::loadDayTextures(std::vector<std::string> filePaths)
{
	dayTexture = loadTextures(filePaths);

	if (dayTexture < 0)
	{
		return false;
	}
	
	return true;
}

bool Skybox::loadNightTextures(std::vector<std::string> filePaths)
{
	nightTexture = loadTextures(filePaths);

	if (nightTexture < 0)
	{
		return false;
	}

	return true;
}


GLuint Skybox::loadTextures(std::vector<std::string> filePaths)
{	
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

	int texCount = 0;
	for (std::string filePath : filePaths)
	{
		int width, height, channels;
		unsigned char* image = SOIL_load_image(filePath.c_str(), &width, &height, &channels, SOIL_LOAD_RGB);
		if (image == 0)
		{
			std::cout << "SOIL failed to load image." << std::endl;
			SOIL_free_image_data(image);
			return -1;
		}

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + texCount, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		SOIL_free_image_data(image);

		texCount++;
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	return texture;
}


Skybox::~Skybox()
{
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteTextures(1, &dayTexture);
	glDeleteTextures(1, &nightTexture);
}
