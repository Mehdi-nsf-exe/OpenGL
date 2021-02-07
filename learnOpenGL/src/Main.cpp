#include <iostream>
#include <cstdlib>
#include <cmath>
#include <map>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#define	STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "Shader.h"
#include "Camera.h"
#include "FPSManager.h"

#include "OpenGLErrorHandling.h"

#define MAJOR_OPENGL_VERSION 4
#define MINOR_OPENGL_VERSION 3

#define WINDOW_WIDTH		800
#define WINDOW_HEIGHT	600

#define TARGET_FPS 35.0

static const char* WINDOW_TITLE = "learnOpenGL";

static const char* OBJECT_VERTEX_SHADER_PATH = "res/shaders/noneLightSrc.vert";
static const char* OBJECT_FRAGMENT_SHADER_PATH = "res/shaders/noneLightSrc.frag";
static const char* LIGHT_VERTEX_SHADER_PATH = "res/shaders/lightSrc.vert";
static const char* LIGHT_FRAGMENT_SHADER_PATH = "res/shaders/lightSrc.frag";
static const char* OUTLINING_VERT_SHADER_PATH = "res/shaders/outliningShader.vert";
static const char* OUTLINING_FRAG_SHADER_PATH = "res/shaders/outliningShader.frag";
static const char* SCREEN_VERT_SHADER = "res/shaders/screenQuadShader.vert";
static const char* SCREEN_FRAG_SHADER = "res/shaders/screenQuadShader.frag";
static const char* SCREEN_KERNEL_FRAG_SHADER = "res/shaders/screenKernelShader.frag";
static const char* SKYBOX_VERT_SHADER_PATH = "res/shaders/skyboxShader.vert";
static const char* SKYBOX_FRAG_SHADER_PATH = "res/shaders/skyboxShader.frag";


static const char* CONTAINER_IMAGE_PATH = "res/textures/container2.png";
static const char* CONTAINER_SPEC_PATH = "res/textures/container2_specular.png";
static const char* GRASS_TEXTURE_PATH = "res/textures/grass.png";
static const char* TRANSPARENT_WINDOW_TEX = "res/textures/transparentWindow.png";
static const char* BLACK_TEXTURE_PATH = "res/textures/blackTexture.png";

static const std::string skyboxTexPaths[] = {
	"res/textures/skybox/right.png",
	"res/textures/skybox/left.png",
	"res/textures/skybox/top.png",
	"res/textures/skybox/bottom.png",
	"res/textures/skybox/front.png",
	"res/textures/skybox/back.png"
};

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
unsigned int loadTexture2D(const char* path);

/**
	Loads a cube map formed buy the images' paths.

	@param paths An array of 6 images' paths.

	@returns the Id of the generated texture.
 */
unsigned int loadCubeMapTex(const std::string paths[]);

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

	float screenQuadVertices[] = {
		// Positions	// TexCoords
		-1.0f, -1.0f,	0.0f, 0.0f,
		 1.0f, -1.0f,	1.0f, 0.0f,
		 1.0f,  1.0f,	1.0f, 1.0f,
		-1.0f,  1.0f,	0.0f, 1.0f
	};

	unsigned int screenQuadIndices[] = {
		0, 1, 2,
		2, 3, 0
	};

	float containerVertices[] = {
		// back face
		-0.5f, -0.5f, -0.5f,	0.0f,  0.0f, -1.0f,  0.0f, 0.0f, // bottom-left
		 0.5f, -0.5f, -0.5f,	0.0f,  0.0f, -1.0f,  1.0f, 0.0f, // bottom-right    
		 0.5f,  0.5f, -0.5f,	0.0f,  0.0f, -1.0f,  1.0f, 1.0f, // top-right              
		 0.5f,  0.5f, -0.5f,	0.0f,  0.0f, -1.0f,  1.0f, 1.0f, // top-right
		-0.5f,  0.5f, -0.5f,	0.0f,  0.0f, -1.0f,  0.0f, 1.0f, // top-left
		-0.5f, -0.5f, -0.5f,	0.0f,  0.0f, -1.0f,  0.0f, 0.0f, // bottom-left                
		// front face
		-0.5f, -0.5f,  0.5f,	0.0f,  0.0f, 1.0f,  0.0f, 0.0f, // bottom-left
		 0.5f,  0.5f,  0.5f,	0.0f,  0.0f, 1.0f,  1.0f, 1.0f, // top-right
		 0.5f, -0.5f,  0.5f,	0.0f,  0.0f, 1.0f,  1.0f, 0.0f, // bottom-right        
		 0.5f,  0.5f,  0.5f,	0.0f,  0.0f, 1.0f,  1.0f, 1.0f, // top-right
		-0.5f, -0.5f,  0.5f,	0.0f,  0.0f, 1.0f,  0.0f, 0.0f, // bottom-left
		-0.5f,  0.5f,  0.5f,	0.0f,  0.0f, 1.0f,  0.0f, 1.0f, // top-left        
		// left face
		-0.5f,  0.5f,  0.5f,	-1.0f, 0.0f, 0.0f,  1.0f, 0.0f, // top-right
		-0.5f, -0.5f, -0.5f,	-1.0f, 0.0f, 0.0f,  0.0f, 1.0f, // bottom-left
		-0.5f,  0.5f, -0.5f,	-1.0f, 0.0f, 0.0f,  1.0f, 1.0f, // top-left       
		-0.5f, -0.5f, -0.5f,	-1.0f, 0.0f, 0.0f,  0.0f, 1.0f, // bottom-left
		-0.5f,  0.5f,  0.5f,	-1.0f, 0.0f, 0.0f,  1.0f, 0.0f, // top-right
		-0.5f, -0.5f,  0.5f,	-1.0f, 0.0f, 0.0f,  0.0f, 0.0f, // bottom-right
		// right face
		 0.5f,  0.5f,  0.5f,	1.0f,  0.0f,  0.0f,  1.0f, 0.0f, // top-left
		 0.5f,  0.5f, -0.5f,	1.0f,  0.0f,  0.0f,  1.0f, 1.0f, // top-right      
		 0.5f, -0.5f, -0.5f,	1.0f,  0.0f,  0.0f,  0.0f, 1.0f, // bottom-right          
		 0.5f, -0.5f, -0.5f,	1.0f,  0.0f,  0.0f,  0.0f, 1.0f, // bottom-right
		 0.5f, -0.5f,  0.5f,	1.0f,  0.0f,  0.0f,  0.0f, 0.0f, // bottom-left
		 0.5f,  0.5f,  0.5f,	1.0f,  0.0f,  0.0f,  1.0f, 0.0f, // top-left
		// bottom face          
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f, // top-right
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f, // bottom-left
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f, // top-left        
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f, // bottom-left
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f, // top-right
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f, // bottom-right
		// top face
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f, // top-left
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f, // top-right
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f, // bottom-right                 
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f, // bottom-right
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f, // bottom-left  
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f  // top-left              
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

	unsigned int groundPlaneIndices[] = {
		0, 1, 2,
		2, 3, 0
	};

	float grassVertices[] = {
		-0.5f,  0.0f,  0.0f,		0.0f, 0.0f, -1.0f,	0.0f, 0.0f,
		 0.5f,  0.0f,  0.0f,		0.0f, 0.0f, -1.0f,	1.0f, 0.0f,
		 0.5f,  1.0f,  0.0f,		0.0f, 0.0f, -1.0f,	1.0f, 1.0f,
		-0.5f,  1.0f,  0.0f,		0.0f, 0.0f, -1.0f,	0.0f, 1.0f
	};

	unsigned int grassIndices[] {
		0, 1, 2,
		2, 3, 0,
	};

	float windowVertices[] = {
		-0.5f, -0.5f,  0.0f,		0.0f, 0.0f, -1.0f,	0.0f, 0.0f,
		 0.5f, -0.5f,  0.0f,		0.0f, 0.0f, -1.0f,	1.0f, 0.0f,
		 0.5f,  0.5f,  0.0f,		0.0f, 0.0f, -1.0f,	1.0f, 1.0f,
		-0.5f,  0.5f,  0.0f,		0.0f, 0.0f, -1.0f,	0.0f, 1.0f,
	};

	unsigned int windowIndices[] = {
		0, 1, 2,
		2, 3, 0,
	};

	float skyboxVertices[] = {
		// Positions
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
	};

	unsigned int skyboxIndices[] = {
		0, 1, 2,
		2, 3, 0,

		4, 2, 0,
		0, 5, 4,

		2, 6, 7,
		7, 3, 2,

		4, 5, 7,
		7, 6, 4,

		0, 3, 7,
		7, 5, 0,

		1, 2, 6,
		6, 4, 1
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

	std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;

	stbi_set_flip_vertically_on_load(true);

	GLCall(glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT));
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mousePosCallback);
	glfwSetScrollCallback(window, scrollCallback);

	GLCall(glEnable(GL_DEPTH_TEST));
	GLCall(glDepthFunc(GL_LEQUAL));
	GLCall(glEnable(GL_STENCIL_TEST));
	GLCall(glEnable(GL_BLEND));
	GLCall(glEnable(GL_CULL_FACE));
	GLCall(glFrontFace(GL_CW)); // It looks like the winding order is the opposit on my computer!!!????

	GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));

	unsigned int screenQuadVaoId;
	GLCall(glGenVertexArrays(1, &screenQuadVaoId));
	GLCall(glBindVertexArray(screenQuadVaoId));
	
	unsigned int screenQuadVboId;
	GLCall(glGenBuffers(1, &screenQuadVboId));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, screenQuadVboId));
	GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(screenQuadVertices), &screenQuadVertices, GL_STATIC_DRAW));

	GLCall(glEnableVertexAttribArray(0));
	GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0));

	GLCall(glEnableVertexAttribArray(1));
	GLCall(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float))));

	unsigned int screenQuadEboId;
	GLCall(glGenBuffers(1, &screenQuadEboId));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, screenQuadEboId));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(screenQuadIndices), &screenQuadIndices, GL_STATIC_DRAW));
	
	GLCall(glBindVertexArray(0));

	GLuint containerVaoId;
	GLCall(glGenVertexArrays(1, &containerVaoId));
	GLCall(glBindVertexArray(containerVaoId));

	GLuint containerVboId;
	GLCall(glGenBuffers(1, &containerVboId));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, containerVboId));
	GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(containerVertices), containerVertices, GL_STATIC_DRAW));

	GLCall(glEnableVertexAttribArray(0));
	GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0));

	GLCall(glEnableVertexAttribArray(1));
	GLCall(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))));

	GLCall(glEnableVertexAttribArray(2));
	GLCall(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))));

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

	unsigned int windowVaoId;
	GLCall(glGenVertexArrays(1, &windowVaoId));
	GLCall(glBindVertexArray(windowVaoId));

	unsigned int windowVboId;
	GLCall(glGenBuffers(1, &windowVboId));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, windowVboId));
	GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(windowVertices), &windowVertices, GL_STATIC_DRAW));

	GLCall(glEnableVertexAttribArray(0));
	GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0));

	GLCall(glEnableVertexAttribArray(1));
	GLCall(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))));

	GLCall(glEnableVertexAttribArray(2));
	GLCall(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))));

	unsigned int windowEboId;
	GLCall(glGenBuffers(1, &windowEboId));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, windowEboId));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(windowIndices), &windowIndices, GL_STATIC_DRAW));

	GLCall(glBindVertexArray(0));

	unsigned int skyboxVaoId;
	GLCall(glGenVertexArrays(1, &skyboxVaoId));
	GLCall(glBindVertexArray(skyboxVaoId));

	unsigned int skyboxVboId;
	GLCall(glGenBuffers(1, &skyboxVboId));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, skyboxVboId));
	GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW));

	GLCall(glEnableVertexAttribArray(0));
	GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0));

	unsigned int skyboxEboId;
	GLCall(glGenBuffers(1, &skyboxEboId));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyboxEboId));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(skyboxIndices), &skyboxIndices, GL_STATIC_DRAW));

	GLCall(glBindVertexArray(0));

	unsigned int fboId;
	GLCall(glGenFramebuffers(1, &fboId));
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, fboId));

	unsigned int fboTextureId;
	GLCall(glGenTextures(1, &fboTextureId));
	GLCall(glBindTexture(GL_TEXTURE_2D, fboTextureId));
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));

	GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboTextureId, 0));
	
	unsigned int fboDepthStencilRboId;
	GLCall(glGenRenderbuffers(1, &fboDepthStencilRboId));
	GLCall(glBindRenderbuffer(GL_RENDERBUFFER, fboDepthStencilRboId));
	GLCall(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WINDOW_WIDTH, WINDOW_HEIGHT));
	GLCall(glBindRenderbuffer(GL_RENDERBUFFER, 0));

	GLCall(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, fboDepthStencilRboId));

	{
		GLenum framebufferStatus;
		GLCall(framebufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER));
		if (framebufferStatus != GL_FRAMEBUFFER_COMPLETE) {
			std::cout << "ERROR::FRAMEBUFFER: Framebuffer not complete." << std::endl;
			return EXIT_FAILURE;
		}
	}
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));

	unsigned int uboPVMats;
	GLCall(glGenBuffers(1, &uboPVMats));

	GLCall(glBindBuffer(GL_UNIFORM_BUFFER, uboPVMats));
	GLCall(glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW));
	GLCall(glBindBuffer(GL_UNIFORM_BUFFER, 0));

	GLCall(glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboPVMats, 0, 2 * sizeof(glm::mat4)));

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

	glm::vec3 windowsPositions[] = {
		glm::vec3(0.0f,  0.0f,  6.0f),
		glm::vec3(-0.0f, -3.5f, -7.0f),
		glm::vec3(2.0f, -3.5f, -3.0f)
	};

	glm::mat4 viewMat;

	glm::mat4 projectionMat = glm::perspective(glm::radians(camera.getZoom()), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);

	GLCall(glBindBuffer(GL_UNIFORM_BUFFER, uboPVMats));
	GLCall(glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projectionMat)));
	GLCall(glBindBuffer(GL_UNIFORM_BUFFER, 0));

	glm::mat4 groundPlaneModelMat(1.0f);
	groundPlaneModelMat = glm::translate(groundPlaneModelMat, glm::vec3(0.0f, -4.0f, -7.0f));
	groundPlaneModelMat = glm::scale(groundPlaneModelMat, glm::vec3(40.0f, 1.0f, 40.0f));

	glm::mat3 groundPlaneNormalMat = glm::mat3(groundPlaneModelMat);
	groundPlaneNormalMat = glm::transpose(glm::inverse(groundPlaneNormalMat));

	glm::mat4 objectModelMat(1.0f);

	unsigned int diffuseMapId;
	unsigned int specularMapId;
	unsigned int grassTextureId;
	unsigned int windowTexId;
	unsigned int blackTextureId;
	unsigned int skyboxTexId;
	
	int mainReturnValue = EXIT_SUCCESS;

	try {

		GLCall(glActiveTexture(GL_TEXTURE0));
		diffuseMapId = loadTexture2D(CONTAINER_IMAGE_PATH);
		GLCall(glBindTexture(GL_TEXTURE_2D, diffuseMapId));
		int containerDiffSampler = 0;

		GLCall(glActiveTexture(GL_TEXTURE1));
		specularMapId = loadTexture2D(CONTAINER_SPEC_PATH);
		GLCall(glBindTexture(GL_TEXTURE_2D, specularMapId));
		int containerSpecSampler = 1;

		GLCall(glActiveTexture(GL_TEXTURE2));
		grassTextureId = loadTexture2D(GRASS_TEXTURE_PATH);
		GLCall(glBindTexture(GL_TEXTURE_2D, grassTextureId));
		int grassSampler = 2;

		GLCall(glActiveTexture(GL_TEXTURE3));
		windowTexId = loadTexture2D(TRANSPARENT_WINDOW_TEX);
		GLCall(glBindTexture(GL_TEXTURE_2D, windowTexId));
		int windowSampler = 3;

		GLCall(glActiveTexture(GL_TEXTURE4));
		blackTextureId = loadTexture2D(BLACK_TEXTURE_PATH);
		GLCall(glBindTexture(GL_TEXTURE_2D, blackTextureId));
		int blackSampler = 4;

		GLCall(glActiveTexture(GL_TEXTURE5));
		GLCall(glBindTexture(GL_TEXTURE_2D, fboTextureId));
		int screenFboTexSampler = 5;

		//stbi_set_flip_vertically_on_load(false);
		GLCall(glActiveTexture(GL_TEXTURE6));
		skyboxTexId = loadCubeMapTex(skyboxTexPaths);
		GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexId));
		int skyboxTexSampler = 6;
		stbi_set_flip_vertically_on_load(true);

		Shader lightSrcShader(LIGHT_VERTEX_SHADER_PATH, LIGHT_FRAGMENT_SHADER_PATH);

		Shader skyboxShader(SKYBOX_VERT_SHADER_PATH, SKYBOX_FRAG_SHADER_PATH);
		skyboxShader.use();
		skyboxShader.setUniform("SkyboxSampler", skyboxTexSampler);

		Shader screenShader(SCREEN_VERT_SHADER, SCREEN_KERNEL_FRAG_SHADER);
		screenShader.use();
		screenShader.setUniform("ScreenSampler", screenFboTexSampler);
		screenShader.setUniform("Offset", 1.0f / 300.0f);
		glm::mat3 kernel(
			0.0f,  0.0f, 0.0f,
			0.0f,  1.0f, 0.0f,
			0.0f,  0.0f, 0.0f
		);

		screenShader.setUniform("Kernel", kernel);

		Shader outliningShader(OUTLINING_VERT_SHADER_PATH, OUTLINING_FRAG_SHADER_PATH);
		outliningShader.use();
		outliningShader.setUniform("OutliningColor", 1.0f, 1.0f, 0.0f, 0.2f);

		Shader noneLightSrcShader(OBJECT_VERTEX_SHADER_PATH, OBJECT_FRAGMENT_SHADER_PATH);
		noneLightSrcShader.use();
		// Material constant uniforms
		noneLightSrcShader.setUniform("material.diffuse", 0);
		noneLightSrcShader.setUniform("material.specular", 1);
		noneLightSrcShader.setUniform("material.shininess", 64.0f);
		// Directional light constant uniforms
		noneLightSrcShader.setUniform("DirLight.direction", 0.0f, -1.0f, 0.0f);
		noneLightSrcShader.setUniform("DirLight.ambient", 0.015f, 0.034f, 0.048f);
		noneLightSrcShader.setUniform("DirLight.diffuse", 0.15f, 0.34f, 0.48f);
		noneLightSrcShader.setUniform("DirLight.specular", 0.15f, 0.34f, 0.48f);
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

		GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

		FPSManager fpsManager(TARGET_FPS);

		while (!glfwWindowShouldClose(window)) {

			fpsManager.startNewFrame();

			updateDeltaTime();
			processInput(window);

			GLCall(glBindFramebuffer(GL_FRAMEBUFFER, fboId));

			GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
			GLCall(glStencilMask(0xFF));
			GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));
			GLCall(glStencilMask(0x00));

			float currentTime = (float)glfwGetTime();

			viewMat = camera.getViewMatrix();

			GLCall(glBindBuffer(GL_UNIFORM_BUFFER, uboPVMats));
			GLCall(glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(viewMat)));
			GLCall(glBindBuffer(GL_UNIFORM_BUFFER, 0));

			lightSrcShader.use();

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
			noneLightSrcShader.setUniform("ViewPos", camera.getPosition());

			noneLightSrcShader.setUniform("FlashLight.pointLightProp.position", camera.getPosition());
			noneLightSrcShader.setUniform("FlashLight.direction", camera.getFront());

			GLCall(glStencilMask(0x00));

			GLCall(glBindVertexArray(groundPlaneVaoId));

			noneLightSrcShader.setUniform("ModelMat", groundPlaneModelMat);
			noneLightSrcShader.setUniform("NormalMat", groundPlaneNormalMat);
			noneLightSrcShader.setUniform("material.diffuse", containerDiffSampler);
			noneLightSrcShader.setUniform("material.specular", containerSpecSampler);

			GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));

			GLCall(glDisable(GL_CULL_FACE));

			GLCall(glBindVertexArray(skyboxVaoId));

			skyboxShader.use();

			GLCall(glDrawElements(GL_TRIANGLES, 6 * 6, GL_UNSIGNED_INT, 0));

			GLCall(glEnable(GL_CULL_FACE));

			GLCall(glBindVertexArray(grassVaoId));
			GLCall(glDisable(GL_CULL_FACE));

			noneLightSrcShader.use();
			noneLightSrcShader.setUniform("material.diffuse", grassSampler);
			noneLightSrcShader.setUniform("material.specular", blackSampler);
			noneLightSrcShader.setUniform("material.shininess", 2.0f);
			
			for (int i = 0; i < 5; i++) {
				glm::mat4 grassModelMat(1.0f);
				grassModelMat = glm::translate(grassModelMat, grassPositions[i]);
				noneLightSrcShader.setUniform("ModelMat", grassModelMat);
				GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));
			}

			GLCall(glEnable(GL_CULL_FACE));
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
			GLCall(glStencilFunc(GL_ALWAYS, 0, 0xFF));

			// Short the transparent windows before drawing.
			std::map<float, glm::vec3> shorted;
			for (int i = 0; i < 3; i++) {
				float distance = glm::length(camera.getPosition() - windowsPositions[i]);
				shorted[distance] = windowsPositions[i];
			}

			GLCall(glBindVertexArray(windowVaoId));
			GLCall(glDisable(GL_CULL_FACE));
			noneLightSrcShader.use();
			noneLightSrcShader.setUniform("material.diffuse", windowSampler);
			noneLightSrcShader.setUniform("material.specular", blackSampler);

			for (std::map<float, glm::vec3>::reverse_iterator it = shorted.rbegin(); it != shorted.rend(); ++it) {
				glm::mat4 windowModelMat(1.0f);
				windowModelMat = glm::translate(windowModelMat, it->second);
				glm::mat3 windowNormalMat = glm::mat3(windowModelMat);
				windowNormalMat = glm::transpose(glm::inverse(windowNormalMat));

				noneLightSrcShader.setUniform("ModelMat", windowModelMat);
				noneLightSrcShader.setUniform("NormalMat", windowNormalMat);

				GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));
			}

			GLCall(glEnable(GL_CULL_FACE));

			GLCall(glBindVertexArray(0));
			GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));

			GLCall(glClearColor(1.0f, 1.0f, 1.0f, 1.0f));
			GLCall(glClear(GL_COLOR_BUFFER_BIT));

			screenShader.use();
			GLCall(glBindVertexArray(screenQuadVaoId));
			GLCall(glDisable(GL_DEPTH_TEST));
			GLCall(glDisable(GL_CULL_FACE));
			
			GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));

			GLCall(glEnable(GL_DEPTH_TEST));
			GLCall(glEnable(GL_CULL_FACE));

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

	GLCall(glDeleteVertexArrays(1, &screenQuadVaoId));
	GLCall(glDeleteBuffers(1, &screenQuadVboId));
	GLCall(glDeleteBuffers(1, &screenQuadEboId));

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
	GLCall(glDeleteTextures(1, &windowTexId));
	GLCall(glDeleteTextures(1, &blackTextureId));

	GLCall(glDeleteFramebuffers(1, &fboId));
	GLCall(glDeleteTextures(1, &fboTextureId));
	GLCall(glDeleteRenderbuffers(1, &fboDepthStencilRboId));

	glfwTerminate();
	return mainReturnValue;
}

unsigned int loadTexture2D(const char* path) {

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

#define CUBE_MAP_FACES_NUM 6

unsigned int loadCubeMapTex(const std::string paths[]) {

	unsigned int textureId;
	GLCall(glGenTextures(1, &textureId));
	GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, textureId));

	int width, height, chanelsNum;
	unsigned char* data;

	for (int i = 0; i < CUBE_MAP_FACES_NUM; i++) {
		data = stbi_load(paths[i].c_str(), &width, &height, &chanelsNum, 0);

		GLenum format;
		switch (chanelsNum) {
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
		if (data) {
			GLCall(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data));
			stbi_image_free(data);
		}
		else {
			throw TextureLoadingFailure();
		}
	}

	GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));

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
