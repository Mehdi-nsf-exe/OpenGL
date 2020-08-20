#pragma once

#include <iostream>
#include <glad/glad.h>

#define ASSERT(x) if (!(x)) __debugbreak();

#define GLCall(x) GLClearError();\
	x;\
	ASSERT(GLLogCall(#x, __FILE__, __LINE__))

/**
	Clears the Error flags of OpenGL.
 */
inline void GLClearError() {
	while (glGetError() != GL_NO_ERROR);
}

/**
	Checks for OpenGL Errors and prints them to the console.
 */
inline bool GLLogCall(const char* function, const char* file, int line) {

	GLenum error = glGetError();

	if (error) {
		std::cout << "[OpenGL Error] (" << error << "): " << function << " " << file << ":" << line << std::endl;
		return false;
	}
	else {
		return true;
	}
}