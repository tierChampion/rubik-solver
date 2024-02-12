#pragma once

#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif

#include <GL/glew.h>
#include <string>

/**
* Single part of a GLSL rendering pipeline
*/
class GLSLShader {
public:
	GLuint _shaderId;
	GLint _compiled;
	GLenum _shaderType;
	std::string _shaderName;

private:
	std::string _shaderSrc;

public:
	GLSLShader();
	GLSLShader(const std::string& shaderName, const char* shaderText, GLenum shaderType);
	GLSLShader(const std::string& shaderName, const std::string& shaderText, GLenum shaderType);
	std::string getSrc() const;
	void setSrc(const std::string& newSource);
	void setSrc(const char* newSource);
	void compile();

private:
	void getCompilationError(GLuint shaderId);
};

