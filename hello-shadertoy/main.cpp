#include <time.h>
#include <fmt/core.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "ShaderProgram.h"

// Set to true to enable fullscreen
bool FULLSCREEN = true;

GLFWwindow* gWindow = NULL;
const char* APP_TITLE = "ShaderToy";

// Window dimensions
const int gWindowWidth = 800;
const int gWindowHeight = 600;

// Fullscreen dimensions
int gWindowWidthFull = 1920;
int gWindowHeightFull = 1200;

bool gWireframe = false;

// Function prototypes
void glfw_onKey(GLFWwindow* window, int key, int scancode, int action, int mode);
void glfw_onFramebufferSize(GLFWwindow* window, int width, int height);
void showFPS(GLFWwindow* window);
bool initOpenGL();


int main(int argc, char **argv)
{
	initOpenGL();

	ShaderProgram shader;
	//shader.loadShaders("shader/main_vert.glsl", "shader/tutorial.glsl");
	//shader.loadShaders("shader/main_vert.glsl", "shader/fire_ball_frag.glsl");
	shader.loadShaders("shader/main_vert.glsl", "shader/unreal_intro_frag.glsl");

	// Set up the rectangle
	//1. Set up an array of vertices for a quad (2 triangls) with an index buffer data
	GLfloat vertices[] = {
		-1.0f,  1.0f, 0.0f,		// Top left
		 1.0f,  1.0f, 0.0f,		// Top right
		 1.0f, -1.0f, 0.0f,		// Bottom right
		-1.0f, -1.0f, 0.0f		// Bottom left 
	};

	GLuint indices[] = {
		0, 1, 2,  // First Triangle
		0, 2, 3   // Second Triangle
	};

	// 2. Set up buffers on the GPU
	GLuint VAO, VBO, IBO;

	glGenVertexArrays(1, &VAO);				// Tell OpenGL to create new Vertex Array Object
	glGenBuffers(1, &VBO);					// Generate an empty vertex buffer on the GPU
	glGenBuffers(1, &IBO);					// Create buffer space on the GPU for the index buffer

	glBindVertexArray(VAO);					// Make it the current one
	glBindBuffer(GL_ARRAY_BUFFER, VBO);		// "bind" or set as the current buffer we are working with

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);	// copy the data from CPU to GPU
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);	// Define a layout for the first vertex buffer "0"
	glEnableVertexAttribArray(0);			// Enable the first attribute or attribute "0"

	// Set up index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Unbind to make sure other code doesn't change it
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glm::vec3 iResolution = glm::vec3(gWindowWidth, gWindowHeight, 0);
	if (FULLSCREEN)
		iResolution = glm::vec3(gWindowWidthFull, gWindowHeightFull, 0);

	clock_t start_time = clock();
	clock_t curr_time;
	float playtime_in_second = 0;

	while (glfwWindowShouldClose(gWindow) == 0) {

		glClear(GL_COLOR_BUFFER_BIT);

		curr_time = clock();
		playtime_in_second = (curr_time - start_time) * 1.0f / 1000.0f;

		shader.use();
		shader.setUniform("iResolution", iResolution);
		shader.setUniform("iTime",playtime_in_second);

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glBindVertexArray(0);

		glfwSwapBuffers(gWindow);
		glfwPollEvents();
	}

	glfwTerminate();
	shader.destroy();

	return 0;
}

// Press ESC to close the window
// Press 1 to toggle wireframe mode
void glfw_onKey(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_1 && action == GLFW_PRESS)
	{
		gWireframe = !gWireframe;
		if (gWireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}

// Is called when the window is resized
void glfw_onFramebufferSize(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

bool initOpenGL()
{
	if (!glfwInit())
	{
		fmt::println("GLFW initialization failed");
		return false;
	}

	// Set the OpenGL version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);	// forward compatible with newer versions of OpenGL as they become available but not backward compatible (it will not run on devices that do not support OpenGL 3.3

	// Create a window
	if (FULLSCREEN)
	{
		const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
		gWindowWidthFull = mode->width;
		gWindowHeightFull = mode->height;
		gWindow = glfwCreateWindow(gWindowWidthFull, gWindowHeightFull, APP_TITLE, glfwGetPrimaryMonitor(), NULL);
	}
	else
		gWindow = glfwCreateWindow(gWindowWidth, gWindowHeight, APP_TITLE, NULL, NULL);

	if (gWindow == NULL)
	{
		fmt::println("Failed to create GLFW window");
		glfwTerminate();
		return false;
	}

	// Make the window's context the current one
	glfwMakeContextCurrent(gWindow);

	gladLoadGL();

	// Set the required callback functions
	glfwSetKeyCallback(gWindow, glfw_onKey);
	glfwSetFramebufferSizeCallback(gWindow, glfw_onFramebufferSize);

	glClearColor(0.23f, 0.38f, 0.47f, 1.0f);

	if (FULLSCREEN)
		glViewport(0, 0, gWindowWidthFull, gWindowHeightFull);
	else
		glViewport(0, 0, gWindowWidth, gWindowHeight);

	return true;
}

void showFPS(GLFWwindow* window) {
	static double previousSeconds = 0.0;
	static int frameCount = 0;
	double elapsedSeconds;
	double currentSeconds = glfwGetTime();

	elapsedSeconds = currentSeconds - previousSeconds;

	if (elapsedSeconds > 0.25) {
		previousSeconds = currentSeconds;
		double fps = (double)frameCount / elapsedSeconds;
		double msPerFrame = 1000.0 / fps;

		char title[80];
		std::snprintf(title, sizeof(title), "Hello Shader @ fps: %.2f, ms/frame: %.2f", fps, msPerFrame);
		glfwSetWindowTitle(window, title);

		frameCount = 0;
	}

	frameCount++;
}
