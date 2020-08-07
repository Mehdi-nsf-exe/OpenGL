#include <iostream>
#include <cstdlib>

#include <glad\glad.h>
#include <GLFW\glfw3.h>

#define MAJOR_OPENGL_VERSION 3
#define MINOR_OPENGL_VERSION 3

#define WINDOW_WIDTH	800
#define WINDOW_HEIGHT	600
#define WINDOW_TITLE	"learnOpenGL"

#define INFOLOG_LENGTH 512

/**
 * The callback for the glfw window resizing event.
 */
void framebufferSizeCallback(GLFWwindow* window, int width, int height);

/** 
 * Processes the input of the window passed as a parameter.
 * @param window the glfw window id of the window whose input to be proccessed.
 */
void processInput(GLFWwindow* window);

int main(void) {

	float vertices[] = {
		 0.5f,  0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f,
		-0.5f,  0.5f, 0.0f,
	};

	unsigned int indices[] = {
		0, 1, 3,
		1, 2, 3
	};

	const char* vertexShaderSrc = "#version 330 core\n"
		"layout (location = 0) in vec3 aPos;\n"

		"void main() {\n"
		"	gl_Position = vec4(aPos.xyz, 1.0);\n"
		"}\n";

	const char* fragmentShaderSrc = "#version 330 core\n"
		"out vec4 fragColor;\n"

		"uniform vec4 ourColor;\n"

		"void main() {\n"
		"	fragColor = ourColor;\n"
		"}\n";

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
		return EXIT_FAILURE;
	}

	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	GLuint vaoId;
	glGenVertexArrays(1, &vaoId);
	glBindVertexArray(vaoId);

	GLuint vboId;
	glGenBuffers(1, &vboId);
	glBindBuffer(GL_ARRAY_BUFFER, vboId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	GLuint eboId;
	glGenBuffers(1, &eboId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	int success;
	char infoLog[INFOLOG_LENGTH];

	GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderId, 1, &vertexShaderSrc, NULL);
	glCompileShader(vertexShaderId);

	glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShaderId, INFOLOG_LENGTH, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		return EXIT_FAILURE;
	}

	GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderId, 1, &fragmentShaderSrc, NULL);
	glCompileShader(fragmentShaderId);

	glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragmentShaderId, INFOLOG_LENGTH, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
		return EXIT_FAILURE;
	}

	GLuint shaderProgramId = glCreateProgram();
	glAttachShader(shaderProgramId, vertexShaderId);
	glAttachShader(shaderProgramId, fragmentShaderId);
	glLinkProgram(shaderProgramId);

	glGetProgramiv(shaderProgramId, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgramId, INFOLOG_LENGTH, NULL, infoLog);
		std::cout << "ERROR::SHADER_PROGRAM::LINK_FAILED\n" << infoLog << std::endl;
		return EXIT_FAILURE;
	}

	glDeleteShader(vertexShaderId);
	glDeleteShader(fragmentShaderId);

	int vertexColorLocation = glGetUniformLocation(shaderProgramId, "ourColor");

	while (!glfwWindowShouldClose(window)) {

		float timeValue = glfwGetTime();
		float rValue = (sin(timeValue / 0.8f) + 1.0f) / 2.0f;
		float gValue = (sin(timeValue / 2.3f) + 1.0f) / 2.0f;
		float bValue = (sin(timeValue / 3.6f) + 1.0f) / 2.0f;

		processInput(window);

		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderProgramId);
		glUniform4f(vertexColorLocation, rValue, gValue, bValue, 1.0f);
		glBindVertexArray(vaoId);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
	return EXIT_SUCCESS;
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}

