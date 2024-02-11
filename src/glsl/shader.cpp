#include "glsl/shader.h"

GLSLShader::GLSLShader() :
	_shaderId(0), _compiled(false), _shaderType(0), _shaderName(""), _shaderSrc("") {}

GLSLShader::GLSLShader(const std::string& shaderName, const char* shaderText, GLenum shaderType) :
	_shaderId(0), _compiled(false), _shaderType(shaderType), _shaderName(shaderName), _shaderSrc(std::string(shaderText)) {}

GLSLShader::GLSLShader(const std::string& shaderName, const std::string& shaderText, GLenum shaderType) :
	_shaderId(0), _compiled(false), _shaderType(shaderType), _shaderName(shaderName), _shaderSrc(shaderText) {}

/**
* @return shader program in text form
*/
std::string GLSLShader::getSrc() const {
	return _shaderSrc;
}

/**
* Set the shader program
* @param newSource - new shader program in text form
*/
void GLSLShader::setSrc(const std::string& newSource) {
	_shaderSrc = newSource;
	_compiled = false;
}

/**
* Set the shader program
* @param newSource - new shader program in text form
*/
void GLSLShader::setSrc(const char* newSource) {
	_shaderSrc = std::string(newSource);
	_compiled = false;
}

/**
* Compile the shader.
*/
void GLSLShader::compile() {
	printf("(S) Compiling shader \"%s\" ... ", this->_shaderName.c_str());
	_shaderId = glCreateShader(_shaderType);
	glShaderSource(_shaderId, 1, ShaderStringHelper(_shaderSrc), NULL);
	glCompileShader(_shaderId);
	/* Error check. */
	glGetShaderiv(_shaderId, GL_COMPILE_STATUS, &_compiled);
	if (!_compiled) {
		getCompilationError(_shaderId);
		glDeleteShader(_shaderId);
		_compiled = false;
	}
	else {
		printf("OK - Shader ID: (%i) \n", _shaderId);
	}
}

/**
* Error callback for the compilation of the shader
* @param shaderId - id of the shader to check the compilation of
*/
void GLSLShader::getCompilationError(GLuint shaderId) {
	int infoLogLength = 0;
	glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, (GLint*)&infoLogLength);
	char* infoLog = (char*)malloc(infoLogLength);
	glGetShaderInfoLog(shaderId, infoLogLength, NULL, infoLog);
	printf("(S) Shader compilation error: \n%s\n", infoLog);
	free(infoLog);
}
