#include <iostream>
#include <vector>
#include <map>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#define GLM_FORCE_RADIANS
#include <GLFW/glfw3.h>

//GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//SOIL
#include <SOIL/SOIL.h>


#include "Shader.h"
#include "Planet.h"

#define PI 3.14159265358979323846
#define TWO_PI 6.28318530717958647693

GLfloat WIDTH = 800, HEIGHT = 600;
GLfloat ASPECT = WIDTH / HEIGHT;

bool KEYS[1024];
glm::vec2 LAST_MOUSE_POS(WIDTH/2, HEIGHT/2);

float SPEED = 1000.0f;
float ROTATE_SPEED = 5.0f;
float MOUSE_ROTATE_SPEED = 0.01;
float PLANET_RADIUS = 10000;

glm::vec3 CAMERA = glm::vec3(-PLANET_RADIUS, -PLANET_RADIUS/2, -PLANET_RADIUS/2);
glm::vec2 CAMERA_ROTATION(0, -PI / 2);

std::map<std::string, GLuint> UNIFORM_LOCATIONS;



void handleControls(GLfloat dt);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void windowFocusCallback(GLFWwindow* window, int focus);
void resizeWindow(GLFWwindow* window, int width, int height);

float round(float f, int numPlaces);

bool loadTexture(std::string filePath, GLuint textureID);

glm::vec2 rotateVector(glm::vec2 v, float angle, glm::vec2 around);


int main()
{
	//Init GLFW
	glfwInit();
	//Set all the required options for GLFW
	glfwDefaultWindowHints();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);


	//Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "PlanetTek", nullptr, nullptr);
	glfwMakeContextCurrent(window);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//Set the required callback functions
	glfwSetKeyCallback(window, keyCallback);
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetWindowFocusCallback(window, windowFocusCallback);
	glfwSetWindowSizeCallback(window, resizeWindow);

	glfwShowWindow(window);

	//Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize GLEW\n";
		return -1;
	}

	//Define the viewport dimensions
	glViewport(0, 0, WIDTH, HEIGHT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);

	GLfloat lastFrame = glfwGetTime();
	GLfloat dt = glfwGetTime();

	Planet planet = Planet(PLANET_RADIUS);
	planet.setPlayerCamera(&CAMERA);

	//Window loop
	while (!glfwWindowShouldClose(window))
	{

		GLfloat currentFrame = glfwGetTime();
		dt = currentFrame - lastFrame;
		lastFrame = currentFrame;

		//Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();
		handleControls(dt);

		planet.update(dt);

		// Clear the colorbuffer
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Setup the Projection and View matricies
		glm::mat4 projection = glm::perspective(45.0f, ASPECT, 1.0f, 10000.0f);
		glm::mat4 view = glm::rotate(CAMERA_ROTATION.x, glm::vec3(1, 0, 0));
		view = view * glm::rotate(CAMERA_ROTATION.y, glm::vec3(0, 1, 0));
		view = view * glm::translate(CAMERA);
		planet.draw(projection, view);


		//Swap the screen buffers
		glfwSwapBuffers(window);
	}

	//Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
	return 0;
}


void handleControls(GLfloat dt)
{
	if (KEYS[GLFW_KEY_W])
	{
		CAMERA.z += glm::cos(CAMERA_ROTATION.y) * SPEED * dt;
		CAMERA.x -= glm::sin(CAMERA_ROTATION.y) * SPEED * dt;
	}
	else if (KEYS[GLFW_KEY_S])
	{
		CAMERA.z -= glm::cos(CAMERA_ROTATION.y) * SPEED * dt;
		CAMERA.x += glm::sin(CAMERA_ROTATION.y) * SPEED * dt;
	}

	if (KEYS[GLFW_KEY_A])
	{
		CAMERA.z += glm::sin(CAMERA_ROTATION.y) * SPEED * dt;
		CAMERA.x += glm::cos(CAMERA_ROTATION.y) * SPEED * dt;
	}
	else if (KEYS[GLFW_KEY_D])
	{
		CAMERA.z -= glm::sin(CAMERA_ROTATION.y) * SPEED * dt;
		CAMERA.x -= glm::cos(CAMERA_ROTATION.y) * SPEED * dt;
	}

	if (KEYS[GLFW_KEY_RIGHT])
	{
		CAMERA_ROTATION.y += ROTATE_SPEED * dt;
	}
	else if (KEYS[GLFW_KEY_LEFT])
	{
		CAMERA_ROTATION.y -= ROTATE_SPEED * dt;
	}

	if (KEYS[GLFW_KEY_SPACE])
	{
		CAMERA.y -= SPEED * dt;
	}
	else if (KEYS[GLFW_KEY_LEFT_SHIFT])
	{
		CAMERA.y += SPEED * dt;
	}

	if (KEYS[GLFW_KEY_F])
	{
		std::cout << "Framerate: " << dt << "ms  " << 1 / dt << "fps" << std::endl;
		KEYS[GLFW_KEY_F] = false;
	}
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
	else if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			KEYS[key] = true;
		}
		else if (action == GLFW_RELEASE)
		{
			KEYS[key] = false;
		}
	}
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	glm::vec2 newMousePos(xpos, ypos);
	glm::vec2 diff = LAST_MOUSE_POS - newMousePos;

	CAMERA_ROTATION.y -= diff.x*MOUSE_ROTATE_SPEED;
	CAMERA_ROTATION.x -= diff.y*MOUSE_ROTATE_SPEED;

	CAMERA_ROTATION.x = glm::min(PI / 2, (double)CAMERA_ROTATION.x);
	CAMERA_ROTATION.x = glm::max(-PI / 2, (double)CAMERA_ROTATION.x);

	LAST_MOUSE_POS = newMousePos;
}

void windowFocusCallback(GLFWwindow* window, int focus)
{
	if (focus)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
	else
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
}

void resizeWindow(GLFWwindow* window, int width, int height)
{
	WIDTH = width;
	HEIGHT = height;
	ASPECT = WIDTH / HEIGHT;

	glViewport(0, 0, WIDTH, HEIGHT);
}

bool loadTexture(std::string filePath, GLuint textureID)
{
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, channels;
	unsigned char* image = SOIL_load_image(filePath.c_str(), &width, &height, &channels, SOIL_LOAD_RGBA);
	if (image == 0)
	{
		std::cout << "SOIL failed to load image." << std::endl;
		return false;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);
	return true;
}

float round(float f, int numPlaces)
{
	if (numPlaces < 0)
	{
		return f;
	}

	for (int i = 0; i < numPlaces; i++)
	{
		f = f * 10;
	}

	f = std::roundf(f);

	for (int i = 0; i < numPlaces; i++)
	{
		f = f / 10;
	}

	return f;
}

glm::vec2 rotateVector(glm::vec2 v, float angle, glm::vec2 around)
{
	angle = glm::radians(angle);
	v -= around;
	v = v * glm::mat2(glm::vec2(glm::cos(angle), glm::sin(angle)),
		glm::vec2(-glm::sin(angle), glm::cos(angle)));
	v += around;

	return v;
}