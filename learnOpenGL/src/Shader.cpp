#include <fstream>
#include <sstream>
#include <iostream>

#include "Shader.h"


char Shader::infoLogBuffer[INFO_LOG_BUFFER_SIZE];

Shader::Shader(const char* vertexPath, const char* fragmentPath) {
	ShaderSrc shaderSrc;
	getShaderSrc(shaderSrc, vertexPath, fragmentPath);
	compileShader(shaderSrc);
}

Shader::Shader(const ShaderSrc& shaderSrc) {
	compileShader(shaderSrc);
}

void Shader::getShaderSrc(ShaderSrc& outShaderSrc, const char* vertexPath, const char* fragmentPath) {

	std::ifstream vertexShaderFile;
	std::ifstream fragmentShaderFile;
	std::stringstream vertexShaderSStream;
	std::stringstream fragmentShaderSStream;

	vertexShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fragmentShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try {
		vertexShaderFile.open(vertexPath);
		fragmentShaderFile.open(fragmentPath);

		vertexShaderSStream << vertexShaderFile.rdbuf();
		fragmentShaderSStream << fragmentShaderFile.rdbuf();

		vertexShaderFile.close();
		fragmentShaderFile.close();
	} catch (...) {
		if (vertexShaderFile.is_open()) {
			vertexShaderFile.close();
		}
		if (fragmentShaderFile.is_open()) {
			fragmentShaderFile.close();
		}
		throw;
	}
	// Return the source code of both the fragmentShader and the vertex shader.
	outShaderSrc.vertexShader = vertexShaderSStream.str();
	outShaderSrc.fragmentShader = fragmentShaderSStream.str();
}

void Shader::compileShader(const ShaderSrc& shaderSrc) {

	unsigned int vertexShaderId, fragmentShaderId;
	int success;

	const char* vertexShaderSrc = shaderSrc.vertexShader.c_str();
	const char* fragmentShaderSrc = shaderSrc.fragmentShader.c_str();

	vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderId, 1, &vertexShaderSrc, NULL);
	glCompileShader(vertexShaderId);

	glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShaderId, INFO_LOG_BUFFER_SIZE, NULL, infoLogBuffer);
		throw VertexShaderCompileError();
	}

	fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderId, 1, &fragmentShaderSrc, NULL);
	glCompileShader(fragmentShaderId);

	glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragmentShaderId, INFO_LOG_BUFFER_SIZE, NULL, infoLogBuffer);
		throw FragmentShaderCompileError();
	}

	programId = glCreateProgram();
	glAttachShader(programId, vertexShaderId);
	glAttachShader(programId, fragmentShaderId);
	glLinkProgram(programId);

	glGetProgramiv(programId, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(programId, INFO_LOG_BUFFER_SIZE, NULL, infoLogBuffer);
		throw ShaderProgramLinkError();
	}

	glDeleteShader(vertexShaderId);
	glDeleteShader(fragmentShaderId);
}

void Shader::setUniform(const std::string& name, bool value) const {
	glUniform1i(glGetUniformLocation(programId, name.c_str()), (int)value);
}

void Shader::setUniform(const std::string& name, int value) const {
	glUniform1i(glGetUniformLocation(programId, name.c_str()), value);
}

void Shader::setUniform(const std::string& name, float value) const {
	glUniform1f(glGetUniformLocation(programId, name.c_str()), value);
}

void Shader::setUniform(const std::string& name, const glm::mat4& matrix) const {
	glUniformMatrix4fv(glGetUniformLocation(programId, name.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
}

// Exceptions:

ShaderCompileError::ShaderCompileError() {
	std::exception();
}

VertexShaderCompileError::VertexShaderCompileError() {
	ShaderCompileError();
}

FragmentShaderCompileError::FragmentShaderCompileError() {
	ShaderCompileError();
}

ShaderProgramLinkError::ShaderProgramLinkError() {
	ShaderCompileError();
}


