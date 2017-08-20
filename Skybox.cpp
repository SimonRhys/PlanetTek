#include "Skybox.h"

Skybox::Skybox()
{
	shader.createShader("skyboxVert.sh", GL_VERTEX_SHADER);
	shader.createShader("skyboxFrag.sh", GL_FRAGMENT_SHADER);
	shader.createProgram();

	UNIFORM_LOCATIONS["projection"] = glGetUniformLocation(shader.getShaderProgram(), "projection");
	UNIFORM_LOCATIONS["view"] = glGetUniformLocation(shader.getShaderProgram(), "view");
	UNIFORM_LOCATIONS["skybox"] = glGetUniformLocation(shader.getShaderProgram(), "skybox");
	UNIFORM_LOCATIONS["fogColour"] = glGetUniformLocation(shader.getShaderProgram(), "fogColour");

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
	view = glm::mat4(glm::mat3(view));
	glUniformMatrix4fv(UNIFORM_LOCATIONS["projection"], 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(UNIFORM_LOCATIONS["view"], 1, GL_FALSE, glm::value_ptr(view));
	glUniform3f(UNIFORM_LOCATIONS["fogColour"], 0.298, 0.5, 0.5);

	glBindVertexArray(vao);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthFunc(GL_LESS);
}

bool Skybox::loadTextures(std::vector<std::string> filePaths)
{	
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
			return false;
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

	glUseProgram(shader.getShaderProgram());
	glUniform1i(UNIFORM_LOCATIONS["skybox"], 0);

	return true;
}


Skybox::~Skybox()
{
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteTextures(1, &texture);
}
