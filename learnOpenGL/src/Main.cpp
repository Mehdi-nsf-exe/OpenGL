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

#include "OpenGLErrorHandling.h"

#define MAJOR_OPENGL_VERSION 3
#define MINOR_OPENGL_VERSION 3

#define WINDOW_WIDTH		800
#define WINDOW_HEIGHT	600

static const char* WINDOW_TITLE = "learnOpenGL";

static const char* OBJECT_VERTEX_SHADER_PATH = "res/shaders/noneLightSrc.vert";
static const char* OBJECT_FRAGMENT_SHADER_PATH = "res/shaders/noneLightSrc.frag";
static const char* LIGHT_VERTEX_SHADER_PATH = "res/shaders/lightSrc.vert";
static const char* LIGHT_FRAGMENT_SHADER_PATH = "res/shaders/lightSrc.frag";

static const char* CONTAINER_IMAGE_PATH = "res/textures/container2.png";
static const char* CONTAINER_SPEC_PATH = "res/textures/container2_specular.png";

static Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

static float deltaTime = 0.0f;
static float lastFrame = 0.0f;

/**
	Exception that indicates an error occured in texture loading.
 */
class TextureLoadingFailure : public std::exception {
public:
	explicit TextureLoadingFailure() {}
};

/**
	Loads the texture from the indicated path.

	@param path The path of the texture image.

	@returns The Id of the texture.
 */
unsigned int loadTexture(const char* path);

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

	float containerVertices[] = {
		// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
	};

	glm::vec3 containerPositions[] = {
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

	float lightSrcVertices[] = {
		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f,

		-0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
	};

	unsigned int lightSrcIndices[] = {
		0, 1, 2,
		2, 3, 0,

		1, 5, 6,
		6, 2, 1,

		5, 4, 7,
		7, 6, 5,

		4, 0, 3,
		3, 7, 4,

		3, 2, 6,
		6, 7, 3,

		1, 0, 4,
		4, 5, 1
	};

	glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

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

	GLCall(glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT));
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mousePosCallback);
	glfwSetScrollCallback(window, scrollCallback);

	GLCall(glEnable(GL_DEPTH_TEST));
	GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
	GLCall(glClear(GL_COLOR_BUFFER_BIT));

	GLuint containerVaoId;
	GLCall(glGenVertexArrays(1, &containerVaoId));
	GLCall(glBindVertexArray(containerVaoId));

	GLuint containerVboId;
	GLCall(glGenBuffers(1, &containerVboId));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, containerVboId));
	GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(containerVertices), containerVertices, GL_STATIC_DRAW));

	GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0));
	GLCall(glEnableVertexAttribArray(0));

	GLCall(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))));
	GLCall(glEnableVertexAttribArray(1));

	GLCall(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))));
	GLCall(glEnableVertexAttribArray(2));

	GLCall(glBindVertexArray(0));

	unsigned int lightSrcVaoId;
	GLCall(glGenVertexArrays(1, &lightSrcVaoId));
	GLCall(glBindVertexArray(lightSrcVaoId));

	unsigned int lightSrcVboId;
	GLCall(glGenBuffers(1, &lightSrcVboId));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, lightSrcVboId));
	GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(lightSrcVertices), &lightSrcVertices, GL_STATIC_DRAW));

	GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0));
	GLCall(glEnableVertexAttribArray(0));

	unsigned int lightSrcEboId;
	GLCall(glGenBuffers(1, &lightSrcEboId));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lightSrcEboId));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(lightSrcIndices), &lightSrcIndices, GL_STATIC_DRAW));

	GLCall(glBindVertexArray(0));

	glm::mat4 viewMat;
	glm::mat4 projectionMat;

	glm::mat4 objectModelMat(1.0f);
	glm::mat3 objectNormalMat = glm::mat3(objectModelMat);
	objectNormalMat = glm::transpose(glm::inverse(objectNormalMat));

	glm::mat4 lightModelMat(1.0f);
	lightModelMat = glm::translate(lightModelMat, lightPos);
	lightModelMat = glm::scale(lightModelMat, glm::vec3(0.2f));

	unsigned int diffuseMapId;
	unsigned int specularMapId;
	
	int mainReturnValue = EXIT_SUCCESS;

	try {

		diffuseMapId = loadTexture(CONTAINER_IMAGE_PATH);
		specularMapId = loadTexture(CONTAINER_SPEC_PATH);

		//Shader lightSrcShader(LIGHT_VERTEX_SHADER_PATH, LIGHT_FRAGMENT_SHADER_PATH);
		//lightSrcShader.use();
		//lightSrcShader.setUniform("LightColor", 1.0f, 1.0f, 1.0f);

		Shader noneLightSrcShader(OBJECT_VERTEX_SHADER_PATH, OBJECT_FRAGMENT_SHADER_PATH);
		noneLightSrcShader.use();
		noneLightSrcShader.setUniform("material.diffuse", 0);
		GLCall(glActiveTexture(GL_TEXTURE0));
		GLCall(glBindTexture(GL_TEXTURE_2D, diffuseMapId));
		noneLightSrcShader.setUniform("material.specular", 1);
		GLCall(glActiveTexture(GL_TEXTURE1));
		GLCall(glBindTexture(GL_TEXTURE_2D, specularMapId));
		noneLightSrcShader.setUniform("material.shininess", 32.0f);

		noneLightSrcShader.setUniform("light.innerCutOff", glm::cos(glm::radians(12.5f)));
		noneLightSrcShader.setUniform("light.outerCutOff", glm::cos(glm::radians(17.5f)));


		noneLightSrcShader.setUniform("light.diffuse", 1.0f, 1.0f, 1.0f);
		noneLightSrcShader.setUniform("light.ambient", 0.1f, 0.1f, 0.1f);
		noneLightSrcShader.setUniform("light.specular", 1.0f, 1.0f, 1.0f);

		noneLightSrcShader.setUniform("light.constant", 1.0f);
		noneLightSrcShader.setUniform("light.linear", 0.045f);
		noneLightSrcShader.setUniform("light.quadratic", 0.0075f);


		while (!glfwWindowShouldClose(window)) {

			updateDeltaTime();
			processInput(window);

			GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

			float currentTime = (float)glfwGetTime();

			viewMat = camera.getViewMatrix();

			projectionMat = glm::perspective(glm::radians(camera.getZoom()), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);

			//lightSrcShader.use();
			//lightSrcShader.setUniform("ModelMat", lightModelMat);
			//lightSrcShader.setUniform("ViewMat", viewMat);
			//lightSrcShader.setUniform("ProjectionMat", projectionMat);
			//
			//glBindVertexArray(lightSrcVaoId);
			//
			//glDrawElements(GL_TRIANGLES, 6 * 6, GL_UNSIGNED_INT, 0);

			noneLightSrcShader.use();
			noneLightSrcShader.setUniform("ViewMat", viewMat);
			noneLightSrcShader.setUniform("ProjectionMat", projectionMat);
			noneLightSrcShader.setUniform("ViewPos", camera.getPosition());

			noneLightSrcShader.setUniform("light.position", camera.getPosition());
			noneLightSrcShader.setUniform("light.direction", camera.getFront());

			GLCall(glBindVertexArray(containerVaoId));

			for (int i = 0; i < 10; i++) {
				objectModelMat = glm::mat4(1.0f);
				objectModelMat = glm::translate(objectModelMat, containerPositions[i]);
				float angle = 20.0f * i;
				objectModelMat = glm::rotate(objectModelMat, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
				noneLightSrcShader.setUniform("ModelMat", objectModelMat);
				GLCall(glDrawArrays(GL_TRIANGLES, 0, 36));
			}

			GLCall(glBindVertexArray(0));

			glfwSwapBuffers(window);
			glfwPollEvents();
		}
	}
	catch (const Shader::VertexShaderCompileError& e) {
		std::cout << "VERTEX SHADER COMPILE ERROR:\n" << Shader::getInfoLogBuffer() << std::endl;
		mainReturnValue = EXIT_FAILURE;
	}
	catch (const Shader::FragmentShaderCompileError& e) {
		std::cout << "FRAGMENT SHADER COMPILE ERROR:\n" << Shader::getInfoLogBuffer() << std::endl;
		mainReturnValue = EXIT_FAILURE;
	}
	catch (const Shader::ShaderProgramLinkError& e) {
		std::cout << "SHADER PROGRAM LINK ERROR:\n" << Shader::getInfoLogBuffer() << std::endl;
		mainReturnValue = EXIT_FAILURE;
	}
	catch (const TextureLoadingFailure & e) {
		std::cout << "TEXTURE LOADING FAILURE\n";
		mainReturnValue = EXIT_FAILURE;
	}
	catch (const std::ios_base::failure& e) {
		std::cout << "FILES COULDN'T BE READ FROM:\n" << e.what() << std::endl;
		mainReturnValue = EXIT_FAILURE;
	}

	GLCall(glDeleteVertexArrays(1, &containerVaoId));
	GLCall(glDeleteBuffers(1, &containerVboId));
	GLCall(glDeleteTextures(1, &diffuseMapId));
	GLCall(glDeleteTextures(1, &specularMapId));
	
	GLCall(glDeleteVertexArrays(1, &lightSrcVaoId));
	GLCall(glDeleteBuffers(1, &lightSrcVboId));
	GLCall(glDeleteBuffers(1, &lightSrcEboId));

	glfwTerminate();
	return mainReturnValue;
}

unsigned int loadTexture(const char* path) {

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data == nullptr) {
		throw TextureLoadingFailure();
	}

	GLenum format;
	switch (nrComponents) {
		case 1: {
			format = GL_RED;
			break;
		} 
		case 3: {
			format = GL_RGB;
			break;
		}
		case 4: {
			format = GL_RGBA;
			break;
		}
		default: {
			ASSERT(false);
		}
	}

	unsigned int textureId;
	GLCall(glGenTextures(1, &textureId));
	GLCall(glBindTexture(GL_TEXTURE_2D, textureId));
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data));
	GLCall(glGenerateMipmap(GL_TEXTURE_2D));

	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

	stbi_image_free(data);
	data = nullptr;

	return textureId;
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
	GLCall(glViewport(0, 0, width, height));
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
