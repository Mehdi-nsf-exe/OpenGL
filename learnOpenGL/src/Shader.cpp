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

	GLCall(vertexShaderId = glCreateShader(GL_VERTEX_SHADER));
	GLCall(glShaderSource(vertexShaderId, 1, &vertexShaderSrc, NULL));
	GLCall(glCompileShader(vertexShaderId));

	GLCall(glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &success));
	if (!success) {
		GLCall(glGetShaderInfoLog(vertexShaderId, INFO_LOG_BUFFER_SIZE, NULL, infoLogBuffer));
		throw VertexShaderCompileError();
	}

	GLCall(fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER));
	GLCall(glShaderSource(fragmentShaderId, 1, &fragmentShaderSrc, NULL));
	GLCall(glCompileShader(fragmentShaderId));

	GLCall(glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &success));
	if (!success) {
		GLCall(glGetShaderInfoLog(fragmentShaderId, INFO_LOG_BUFFER_SIZE, NULL, infoLogBuffer));
		throw FragmentShaderCompileError();
	}

	GLCall(programId = glCreateProgram());
	GLCall(glAttachShader(programId, vertexShaderId));
	GLCall(glAttachShader(programId, fragmentShaderId));
	GLCall(glLinkProgram(programId));

	GLCall(glGetProgramiv(programId, GL_LINK_STATUS, &success));
	if (!success) {
		GLCall(glGetProgramInfoLog(programId, INFO_LOG_BUFFER_SIZE, NULL, infoLogBuffer));
		throw ShaderProgramLinkError();
	}

	GLCall(glDeleteShader(vertexShaderId));
	GLCall(glDeleteShader(fragmentShaderId));
}

int Shader::getUniformLocation(const std::string& name) const {
	int location;
	GLCall(location = glGetUniformLocation(programId, name.c_str()));
	if (location == -1) {
		throw InvalidUniformLocation();
	}
	return location;
}

void Shader::setUniform(const std::string& name, bool value) const {
	int location = getUniformLocation(name);
	GLCall(glUniform1i(location, (int)value));
}

void Shader::setUniform(const std::string& name, int value) const {
	int location = getUniformLocation(name);
	GLCall(glUniform1i(location, value));
}

void Shader::setUniform(const std::string& name, float value) const {
	int location = getUniformLocation(name);
	GLCall(glUniform1f(location, value));
}

void Shader::setUniform(const std::string& name, const glm::mat4& matrix) const {
	int location = getUniformLocation(name);
	GLCall(glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix)));
}

void Shader::setUniform(const std::string& name, const glm::mat3& matrix) const {
	int location = getUniformLocation(name);
	GLCall(glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix)));
}

void Shader::setUniform(const std::string& name, const glm::vec3& vector) const {
	int location = getUniformLocation(name);
	GLCall(glUniform3fv(location, 1, glm::value_ptr(vector)));
}

void Shader::setUniform(const std::string& name, float x, float y, float z) const {
	int location = getUniformLocation(name);
	GLCall(glUniform3f(location, x, y, z));
}


