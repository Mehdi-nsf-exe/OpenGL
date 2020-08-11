#pragma once

#include <string>
#include <memory>

#include <glad\glad.h>

#define INFO_LOG_BUFFER_SIZE 512

/** 
	the source code of an openGL shader program.
 */
struct ShaderSrc {
	std::string vertexShader;
	std::string fragmentShader;
};

/** 
	OpenGL shader program.
 */
class Shader {
private:

	unsigned int programId;
	static char infoLogBuffer[INFO_LOG_BUFFER_SIZE];

public:

	/**
		Reads the source code of the shader and builds it.
		@param vertexPath The path of the vertex shader's source code.
		@param fragmentPath The path of the fragment shader's source code.
		@throws std::ifstream::failure if the file is not succesfully read.
		@throws FertexShaderCompileError, FragmentShaderCompileError, ShaderProgramLinkError If any of the corresponding errors
		accured during shader compilation.
	 */
	Shader(const char* vertexPath, const char* fragmentPath);

	/**
		Creates and compiles a shader program from the indicated shader source code.
		@param shaderSrc The struct that contains the shader source code.
		@throws FertexShaderCompileError, FragmentShaderCompileError, ShaderProgramLinkError If any of the corresponding errors
		accured during shader compilation.
	 */
	Shader(const ShaderSrc& shaderSrc);

	/**
		Use/activate the shader. 
	 */
	inline void use() const {
		glUseProgram(programId);
	}

	/**
		utility uniform funciontions that set the indicated uniforms to the indicated values.
		@param name The name of the uniform.
		@param value The value to set the uniform to.
	 */
	void setUniform(const std::string& name, bool value) const;
	void setUniform(const std::string& name, int value) const;
	void setUniform(const std::string& name, float value) const;

	/**
		Reads the shader program source code form the indicated files.
		@param outShaderSrc The reference to the ShaderSrc struct in which to save the source code of the shaders.
		@param vertexPath The Path of the vertex shader's source code.
		@param fragmentPath The path of the fragment shader's source code.
	 */
	static void getShaderSrc(ShaderSrc& outShaderSrc, const char* vertexPath, const char* fragmentPath);

	/**
		Gets a pointer to the info log buffer.
	 */
	static inline const char* getInfoLogBuffer() {
		return infoLogBuffer;
	}

private:

	/**
		Compiles and links the shader program from the indicated shader source.
		@param shaderSrc The shader's source code.
	 */
	void compileShader(const ShaderSrc& shaderSrc);
};


/**
	Exception that indicates that an error occured in the shader program compilation/linking.
 */
class ShaderCompileError : public std::exception {
public:
	/**
		Creates and initializes a ShaderCompileError.
	 */
	explicit ShaderCompileError();

};

/**
	Exception that indicates that an error occured in the vertex shader compilation.
 */
class VertexShaderCompileError : public ShaderCompileError {
public:
	/**
		Creates a vertex shader compile error.
	 */
	explicit VertexShaderCompileError();
};

/**
	Exception that indicates that an error occured in the fragment shader compilation.
 */
class FragmentShaderCompileError : public ShaderCompileError {
public:
	/**
		Creates the fragment shader compile error.
	 */
	explicit FragmentShaderCompileError();
};

/**
	Exception that indicates that an error occured in the shader program linking.
 */
class ShaderProgramLinkError : public ShaderCompileError {
public:
	/**
		Creates the shader porgram link error.
	 */
	explicit ShaderProgramLinkError();
};

