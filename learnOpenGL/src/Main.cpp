#include <iostream>
#include <cstdlib>
#include <cmath>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#define	STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "Shader.h"
#include "Camera.h"

#define MAJOR_OPENGL_VERSION 3
#define MINOR_OPENGL_VERSION 3

#define WINDOW_WIDTH		800
#define WINDOW_HEIGHT	600
#define WINDOW_TITLE		"learnOpenGL"

#define VERTEX_SHADER_PATH		"res\\shaders\\shader.vert"
#define FRAGMENT_SHADER_PATH	"res\\shaders\\shader.frag"
#define CONTAINER_IMAGE_PATH	"res\\textures\\container.jpg"
#define FACE_IMAGE_PATH			"res\\textures\\awesomeface.png"

#define TEXTURES_COUNT				2
#define CONTAINER_TEXTURE_INDEX	0
#define FACE_TEXTURE_INDEX			1

static Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

static float deltaTime = 0.0f;
static float lastFrame = 0.0f;

/**
	The callback for the glfw window resizing event.
 */
void framebufferSizeCallback(GLFWwindow* window, int width, int height);

/**
	the mouse callback for the glfw mouse position input.
 */
void mousePosCallback(GLFWwindow* window, double xPos, double yPos);

/**
	the scroll callback for the glfw scroll input.
 */
void scrollCallback(GLFWwindow* window, double xOffset, double yOffset);

/** 
	Processes the input of the window passed as a parameter.
	@param window the glfw window id of the window whose input to be proccessed.
 */
void processInput(GLFWwindow* window);

/**
	updates the deltaTime every frame if the funcion is called in the game loop.
*/
void updateDeltaTime();

int main(void) {

	float vertices[] = {
	 -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
	  0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
	  0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	  0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	 -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

	 -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	  0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	  0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	  0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	 -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
	 -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

	 -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	  0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	  0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	  0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	  0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	  0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	  0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	 -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	  0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
	  0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	  0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

	 -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	  0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	  0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	  0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
	 -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};

	glm::vec3 cubePositiones[] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};

	unsigned int indices[] = {
		0, 1, 3,
		1, 2, 3
	};

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, MAJOR_OPENGL_VERSION);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, MINOR_OPENGL_VERSION);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, nullptr, nullptr);
	if (window == nullptr) {
		std::cout << "failed to creat GLFW window" << std::endl;
		glfwTerminate();
		return EXIT_FAILURE;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		glfwTerminate();
		return EXIT_FAILURE;
	}

	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mousePosCallback);
	glfwSetScrollCallback(window, scrollCallback);

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	GLuint vaoId;
	glGenVertexArrays(1, &vaoId);
	glBindVertexArray(vaoId);

	GLuint vboId;
	glGenBuffers(1, &vboId);
	glBindBuffer(GL_ARRAY_BUFFER, vboId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	GLuint eboId;
	glGenBuffers(1, &eboId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	int imageWidth, imageHeight, nrChannels;
	unsigned char* imageData;
	stbi_set_flip_vertically_on_load(true);

	unsigned int textureIds[TEXTURES_COUNT];
	glGenTextures(TEXTURES_COUNT, textureIds);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureIds[CONTAINER_TEXTURE_INDEX]);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	imageData = stbi_load(CONTAINER_IMAGE_PATH, &imageWidth, &imageHeight, &nrChannels, 0);
	if (imageData) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(imageData);
	imageData = nullptr;
	
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textureIds[FACE_TEXTURE_INDEX]);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	imageData = stbi_load(FACE_IMAGE_PATH, &imageWidth, &imageHeight, &nrChannels, 0);
	if (imageData) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(imageData);

	glm::mat4 viewM;

	glm::mat4 projectionM;

	const float camTrajectoryRadius = 10.0f;
	
	int mainReturnValue = EXIT_SUCCESS;

	try {
		Shader shader(VERTEX_SHADER_PATH, FRAGMENT_SHADER_PATH);

		shader.use();
		shader.setUniform("texture0", 0);
		shader.setUniform("texture1", 1);

		while (!glfwWindowShouldClose(window)) {

			updateDeltaTime();
			processInput(window);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			float currentTime = (float)glfwGetTime();

			viewM = camera.getViewMatrix();
			shader.setUniform("viewM", viewM);

			projectionM = glm::perspective(glm::radians(camera.getZoom()), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);
			shader.setUniform("projectionM", projectionM);

			float rotationAngle = currentTime * glm::radians(50.0f);

			glBindVertexArray(vaoId);
			for (unsigned int i = 0; i < 10; i++) {
				glm::mat4 modelM(1.0f);
				modelM = glm::translate(modelM, cubePositiones[i]);
				float phaseAngle = 20.0f * i;
				modelM = glm::rotate(modelM, rotationAngle + phaseAngle, glm::vec3(1.0f, 0.3f, 0.5f));
				shader.setUniform("modelM", modelM);

				glDrawArrays(GL_TRIANGLES, 0, 36);
			}
			glBindVertexArray(0);

			glfwSwapBuffers(window);
			glfwPollEvents();
		}
	}
	catch (const VertexShaderCompileError& e) {
		std::cout << "VERTEX SHADER COMPILE ERROR:\n" << Shader::getInfoLogBuffer() << std::endl;
		mainReturnValue = EXIT_FAILURE;
	}
	catch (const FragmentShaderCompileError& e) {
		std::cout << "FRAGMENT SHADER COMPILE ERROR:\n" << Shader::getInfoLogBuffer() << std::endl;
		mainReturnValue = EXIT_FAILURE;
	}
	catch (const ShaderProgramLinkError& e) {
		std::cout << "SHADER PROGRAM LINK ERROR:\n" << Shader::getInfoLogBuffer() << std::endl;
		mainReturnValue = EXIT_FAILURE;
	}
	catch (const std::ios_base::failure& e) {
		std::cout << "FILES COULDN'T BE READ FROM:\n" << e.what() << std::endl;
		mainReturnValue = EXIT_FAILURE;
	}
	glfwTerminate();
	return mainReturnValue;
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
	
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		camera.move(CameraMovement::FORWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		camera.move(CameraMovement::BACKWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		camera.move(CameraMovement::LEFT, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		camera.move(CameraMovement::RIGHT, deltaTime);
	}
}

void updateDeltaTime() {
	float currentFrame = (float)glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
}

void mousePosCallback(GLFWwindow* window, double xPos, double yPos) {

	static bool firstMouse = true;
	static float lastX = 0.0f;
	static float lastY = 0.0f;

	if (firstMouse) {
		lastX = xPos;
		lastY = yPos;
		firstMouse = false;
	}

	float xOffset = xPos - lastX;
	float yOffset = lastY - yPos;

	lastX = xPos;
	lastY = yPos;

	camera.updateOrientation(xOffset, yOffset);
}

void scrollCallback(GLFWwindow* window, double xOffset, double yOffset) {
	camera.updateZoom((float)yOffset);
}
