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
static const char* OUTLINING_VERT_SHADER_PATH = "res/shaders/outliningShader.vert";
static const char* OUTLINING_FRAG_SHADER_PATH = "res/shaders/outliningShader.frag";


static const char* CONTAINER_IMAGE_PATH = "res/textures/container2.png";
static const char* CONTAINER_SPEC_PATH = "res/textures/container2_specular.png";
static const char* GRASS_TEXTURE_PATH = "res/textures/grass.png";
static const char* BLACK_TEXTURE_PATH = "res/textures/blackTexture.png";

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

	float groundPlaneVertices[] = {
		// Positions			// normals			// Texture coords
		-0.5f,  0.0f, -0.5f,	0.0f,  1.0f, 0.0f,	0.0f,  0.0f,
		 0.5f,  0.0f, -0.5f,	0.0f,  1.0f, 0.0f,	20.0f, 0.0f,
		 0.5f,  0.0f,  0.5f,	0.0f,  1.0f, 0.0f,	20.0f, 20.0f,
		-0.5f,  0.0f,  0.5f,	0.0f,  1.0f, 0.0f,	0.0f,  20.0f
	};

	int groundPlaneIndices[] = {
		0, 1, 2,
		2, 3, 0
	};

	float grassVertices[] = {
		-0.5f,  0.0f,  0.0f,		0.0f, 0.0f, -1.0f,	0.0f, 0.0f,
		 0.5f,  0.0f,  0.0f,		0.0f, 0.0f, -1.0f,	1.0f, 0.0f,
		 0.5f,  1.0f,  0.0f,		0.0f, 0.0f, -1.0f,	1.0f, 1.0f,
		-0.5f,  1.0f,  0.0f,		0.0f, 0.0f, -1.0f,	0.0f, 1.0f

		-0.5f,  0.0f,  0.0f,		0.0f, 0.0f,  1.0f,	0.0f, 0.0f,
		 0.5f,  0.0f,  0.0f,		0.0f, 0.0f,  1.0f,	1.0f, 0.0f,
		 0.5f,  1.0f,  0.0f,		0.0f, 0.0f,  1.0f,	1.0f, 1.0f,
		-0.5f,  1.0f,  0.0f,		0.0f, 0.0f,  1.0f,	0.0f, 1.0f
	};

	int grassIndices[] {
		0, 1, 2,
		2, 3, 0,

		5, 4, 7,
		7, 6, 5
	};

	glm::vec3 grassPositions[]{
		glm::vec3(-1.5f, -4.0f, -7.48f),
		glm::vec3( 1.5f, -4.0f, -6.49f),
		glm::vec3( 0.0f, -4.0f, -6.3f),
		glm::vec3(-0.3f, -4.0f, -9.3f),
		glm::vec3( 0.5f, -4.0f, -7.6f)
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

	stbi_set_flip_vertically_on_load(true);

	GLCall(glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT));
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mousePosCallback);
	glfwSetScrollCallback(window, scrollCallback);

	GLCall(glEnable(GL_DEPTH_TEST));
	GLCall(glEnable(GL_STENCIL_TEST));
	GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));

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

	unsigned int groundPlaneVaoId;
	GLCall(glGenVertexArrays(1, &groundPlaneVaoId));
	GLCall(glBindVertexArray(groundPlaneVaoId));

	unsigned int groundPlaneVboId;
	GLCall(glGenBuffers(1, &groundPlaneVboId));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, groundPlaneVboId));
	GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(groundPlaneVertices), &groundPlaneVertices, GL_STATIC_DRAW));

	GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0));
	GLCall(glEnableVertexAttribArray(0));
	
	GLCall(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))));
	GLCall(glEnableVertexAttribArray(1));
	
	GLCall(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))));
	GLCall(glEnableVertexAttribArray(2));

	unsigned int groundPlaneEboId;
	GLCall(glGenBuffers(1, &groundPlaneEboId));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, groundPlaneEboId));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(groundPlaneIndices), &groundPlaneIndices, GL_STATIC_DRAW));

	GLCall(glBindVertexArray(0));

	unsigned int grassVaoId;
	GLCall(glGenVertexArrays(1, &grassVaoId));
	GLCall(glBindVertexArray(grassVaoId));

	unsigned int grassVboId;
	GLCall(glGenBuffers(1, &grassVboId));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, grassVboId));
	GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(grassVertices), &grassVertices, GL_STATIC_DRAW));

	GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0));
	GLCall(glEnableVertexAttribArray(0));

	GLCall(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))));
	GLCall(glEnableVertexAttribArray(1));

	GLCall(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))));
	GLCall(glEnableVertexAttribArray(2));

	unsigned int grassEboId;
	GLCall(glGenBuffers(1, &grassEboId));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, grassEboId));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(grassIndices), &grassIndices, GL_STATIC_DRAW));

	GLCall(glBindVertexArray(0));

	glm::vec3 pointLightsPositions[] = {
		glm::vec3(-4.0f,  2.0f, -12.0f),
		glm::vec3( 2.3f, -3.3f, -4.0f),
		glm::vec3( 0.0f,  0.0f,  3.0f),
		glm::vec3( 0.7f,  0.2f,  2.0f)
	};

	glm::vec3 pointLightsColors[] = {
		glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 1.0f)
	};

	glm::mat4 viewMat;
	glm::mat4 projectionMat;

	glm::mat4 groundPlaneModelMat(1.0f);
	groundPlaneModelMat = glm::translate(groundPlaneModelMat, glm::vec3(0.0f, -4.0f, -7.0f));
	groundPlaneModelMat = glm::scale(groundPlaneModelMat, glm::vec3(40.0f, 1.0f, 40.0f));

	glm::mat3 groundPlaneNormalMat = glm::mat3(groundPlaneModelMat);
	groundPlaneNormalMat = glm::transpose(glm::inverse(groundPlaneNormalMat));

	glm::mat4 objectModelMat(1.0f);

	unsigned int diffuseMapId;
	unsigned int specularMapId;
	unsigned int grassTextureId;
	unsigned int blackTextureId;
	
	int mainReturnValue = EXIT_SUCCESS;

	try {

		GLCall(glActiveTexture(GL_TEXTURE0));
		diffuseMapId = loadTexture(CONTAINER_IMAGE_PATH);
		GLCall(glBindTexture(GL_TEXTURE_2D, diffuseMapId));
		int containerDiffSampler = 0;

		GLCall(glActiveTexture(GL_TEXTURE1));
		specularMapId = loadTexture(CONTAINER_SPEC_PATH);
		GLCall(glBindTexture(GL_TEXTURE_2D, specularMapId));
		int containerSpecSampler = 1;

		GLCall(glActiveTexture(GL_TEXTURE2));
		grassTextureId = loadTexture(GRASS_TEXTURE_PATH);
		GLCall(glBindTexture(GL_TEXTURE_2D, grassTextureId));
		int grassSampler = 2;

		GLCall(glActiveTexture(GL_TEXTURE3));
		blackTextureId = loadTexture(BLACK_TEXTURE_PATH);
		GLCall(glBindTexture(GL_TEXTURE_2D, blackTextureId));
		int blackSampler = 3;

		Shader lightSrcShader(LIGHT_VERTEX_SHADER_PATH, LIGHT_FRAGMENT_SHADER_PATH);

		Shader outliningShader(OUTLINING_VERT_SHADER_PATH, OUTLINING_FRAG_SHADER_PATH);
		outliningShader.use();
		outliningShader.setUniform("OutliningColor", 1.0f, 1.0f, 0.0f, 1.0f);

		Shader noneLightSrcShader(OBJECT_VERTEX_SHADER_PATH, OBJECT_FRAGMENT_SHADER_PATH);
		noneLightSrcShader.use();
		// Material constant uniforms
		noneLightSrcShader.setUniform("material.diffuse", 0);
		noneLightSrcShader.setUniform("material.specular", 1);
		noneLightSrcShader.setUniform("material.shininess", 64.0f);
		// Directional light constant uniforms
		noneLightSrcShader.setUniform("DirLight.direction", 0.0f, -1.0f, 0.0f);
		noneLightSrcShader.setUniform("DirLight.ambient", 0.02f, 0.03f, 0.03f);
		noneLightSrcShader.setUniform("DirLight.diffuse", 0.2f, 0.3f, 0.3f);
		noneLightSrcShader.setUniform("DirLight.specular", 0.2f, 0.3f, 0.3f);
		// Flash light constant uniforms
		noneLightSrcShader.setUniform("FlashLight.pointLightProp.diffuse", 0.8f, 1.0f, 0.2f);
		noneLightSrcShader.setUniform("FlashLight.pointLightProp.ambient", 0.08f, 0.1f, 0.02f);
		noneLightSrcShader.setUniform("FlashLight.pointLightProp.specular", 0.8f, 1.0f, 0.2f);
		noneLightSrcShader.setUniform("FlashLight.pointLightProp.constant", 1.0f);
		noneLightSrcShader.setUniform("FlashLight.pointLightProp.linear", 0.14f);
		noneLightSrcShader.setUniform("FlashLight.pointLightProp.quadratic", 0.07f);
		noneLightSrcShader.setUniform("FlashLight.innerCutOff", glm::cos(glm::radians(12.5f)));
		noneLightSrcShader.setUniform("FlashLight.outerCutOff", glm::cos(glm::radians(17.5f)));
		// point lights constant uniforms
		noneLightSrcShader.setUniform("PointLights[0].position", pointLightsPositions[0]);
		noneLightSrcShader.setUniform("PointLights[0].ambient", pointLightsColors[0] * 0.1f);
		noneLightSrcShader.setUniform("PointLights[0].diffuse", pointLightsColors[0]);
		noneLightSrcShader.setUniform("PointLights[0].specular", pointLightsColors[0]);
		noneLightSrcShader.setUniform("PointLights[0].constant", 1.0f);
		noneLightSrcShader.setUniform("PointLights[0].linear", 0.007f);
		noneLightSrcShader.setUniform("PointLights[0].quadratic", 0.0002f);

		noneLightSrcShader.setUniform("PointLights[1].position", pointLightsPositions[1]);
		noneLightSrcShader.setUniform("PointLights[1].ambient", pointLightsColors[1] * 0.1f);
		noneLightSrcShader.setUniform("PointLights[1].diffuse", pointLightsColors[1]);
		noneLightSrcShader.setUniform("PointLights[1].specular", pointLightsColors[1]);
		noneLightSrcShader.setUniform("PointLights[1].constant", 1.0f);
		noneLightSrcShader.setUniform("PointLights[1].linear", 0.014f);
		noneLightSrcShader.setUniform("PointLights[1].quadratic", 0.0007f);

		noneLightSrcShader.setUniform("PointLights[2].position", pointLightsPositions[2]);
		noneLightSrcShader.setUniform("PointLights[2].ambient", pointLightsColors[2] * 0.1f);
		noneLightSrcShader.setUniform("PointLights[2].diffuse", pointLightsColors[2]);
		noneLightSrcShader.setUniform("PointLights[2].specular", pointLightsColors[2]);
		noneLightSrcShader.setUniform("PointLights[2].constant", 1.0f);
		noneLightSrcShader.setUniform("PointLights[2].linear", 0.022f);
		noneLightSrcShader.setUniform("PointLights[2].quadratic", 0.0019f);

		noneLightSrcShader.setUniform("PointLights[3].position", pointLightsPositions[3]);
		noneLightSrcShader.setUniform("PointLights[3].ambient", pointLightsColors[3] * 0.1f);
		noneLightSrcShader.setUniform("PointLights[3].diffuse", pointLightsColors[3]);
		noneLightSrcShader.setUniform("PointLights[3].specular", pointLightsColors[3]);
		noneLightSrcShader.setUniform("PointLights[3].constant", 1.0f);
		noneLightSrcShader.setUniform("PointLights[3].linear", 0.027f);
		noneLightSrcShader.setUniform("PointLights[3].quadratic", 0.0028f);

		while (!glfwWindowShouldClose(window)) {

			updateDeltaTime();
			processInput(window);

			GLCall(glStencilMask(0xFF));
			GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));
			GLCall(glStencilMask(0x00));

			float currentTime = (float)glfwGetTime();

			viewMat = camera.getViewMatrix();

			projectionMat = glm::perspective(glm::radians(camera.getZoom()), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);

			lightSrcShader.use();
			lightSrcShader.setUniform("ViewMat", viewMat);
			lightSrcShader.setUniform("ProjectionMat", projectionMat);

			GLCall(glBindVertexArray(lightSrcVaoId));

			for (int i = 0; i < 4; i++) {
				glm::mat4 lightModelMat(1.0f);
				lightModelMat = glm::translate(lightModelMat, pointLightsPositions[i]);
				lightModelMat = glm::scale(lightModelMat, glm::vec3(0.2f));
				lightSrcShader.setUniform("ModelMat", lightModelMat);

				lightSrcShader.setUniform("LightColor", pointLightsColors[i]);

				GLCall(glDrawElements(GL_TRIANGLES, 6 * 6, GL_UNSIGNED_INT, 0));
			}

			noneLightSrcShader.use();
			noneLightSrcShader.setUniform("ViewMat", viewMat);
			noneLightSrcShader.setUniform("ProjectionMat", projectionMat);
			noneLightSrcShader.setUniform("ViewPos", camera.getPosition());

			noneLightSrcShader.setUniform("FlashLight.pointLightProp.position", camera.getPosition());
			noneLightSrcShader.setUniform("FlashLight.direction", camera.getFront());

			GLCall(glStencilMask(0x00));

			GLCall(glBindVertexArray(groundPlaneVaoId));

			noneLightSrcShader.setUniform("ModelMat", groundPlaneModelMat);
			noneLightSrcShader.setUniform("NormalMat", groundPlaneNormalMat);
			GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));

			GLCall(glBindVertexArray(grassVaoId));

			noneLightSrcShader.use();
			noneLightSrcShader.setUniform("material.diffuse", grassSampler);
			noneLightSrcShader.setUniform("material.specular", blackSampler);
			noneLightSrcShader.setUniform("material.shininess", 2.0f);
			
			for (int i = 0; i < 5; i++) {
				glm::mat4 grassModelMat(1.0f);
				grassModelMat = glm::translate(grassModelMat, grassPositions[i]);
				noneLightSrcShader.setUniform("ModelMat", grassModelMat);
				GLCall(glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0));
			}

			GLCall(glBindVertexArray(containerVaoId));

			GLCall(glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE));
			GLCall(glStencilMask(0xFF));
			GLCall(glStencilFunc(GL_ALWAYS, 1, 0xFF));

			noneLightSrcShader.use();
			noneLightSrcShader.setUniform("material.diffuse", containerDiffSampler);
			noneLightSrcShader.setUniform("material.specular", containerSpecSampler);
			noneLightSrcShader.setUniform("material.shininess", 32.0f);

			for (int i = 0; i < 10; i++) {
				objectModelMat = glm::mat4(1.0f);
				objectModelMat = glm::translate(objectModelMat, containerPositions[i]);
				float angle = 20.0f * i;
				objectModelMat = glm::rotate(objectModelMat, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));


				glm::mat3 objectNormalMat = glm::mat3(objectModelMat);
				objectNormalMat = glm::transpose(glm::inverse(objectNormalMat));

				noneLightSrcShader.setUniform("ModelMat", objectModelMat);
				noneLightSrcShader.setUniform("NormalMat", objectNormalMat);
				GLCall(glDrawArrays(GL_TRIANGLES, 0, 36));
			}

			GLCall(glStencilFunc(GL_NOTEQUAL, 1, 0xFF));
			GLCall(glStencilMask(0x00));

			outliningShader.use();
			outliningShader.setUniform("ViewMat", viewMat);
			outliningShader.setUniform("ProjectionMat", projectionMat);

			for (int i = 0; i < 10; i++) {
				objectModelMat = glm::mat4(1.0f);
				objectModelMat = glm::translate(objectModelMat, containerPositions[i]);
				objectModelMat = glm::scale(objectModelMat, glm::vec3(1.05f, 1.05f, 1.05f));
				float angle = 20.0f * i;
				objectModelMat = glm::rotate(objectModelMat, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));

				outliningShader.setUniform("ModelMat", objectModelMat);
				GLCall(glDrawArrays(GL_TRIANGLES, 0, 36));
			}

			GLCall(glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP));
			GLCall(glStencilMask(0x00));

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
	catch (const TextureLoadingFailure& e) {
		std::cout << "TEXTURE LOADING FAILURE\n";
		mainReturnValue = EXIT_FAILURE;
	}
	catch (const std::ios_base::failure& e) {
		std::cout << "FILES COULDN'T BE READ FROM:\n" << e.what() << std::endl;
		mainReturnValue = EXIT_FAILURE;
	}

	GLCall(glDeleteVertexArrays(1, &containerVaoId));
	GLCall(glDeleteBuffers(1, &containerVboId));
	
	GLCall(glDeleteVertexArrays(1, &lightSrcVaoId));
	GLCall(glDeleteBuffers(1, &lightSrcVboId));
	GLCall(glDeleteBuffers(1, &lightSrcEboId));

	GLCall(glDeleteVertexArrays(1, &groundPlaneVaoId));
	GLCall(glDeleteBuffers(1, &groundPlaneVboId));
	GLCall(glDeleteBuffers(1, &groundPlaneEboId));

	GLCall(glDeleteTextures(1, &diffuseMapId));
	GLCall(glDeleteTextures(1, &specularMapId));
	GLCall(glDeleteTextures(1, &grassTextureId));
	GLCall(glDeleteTextures(1, &blackTextureId));

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
