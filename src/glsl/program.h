#pragma once
#include "glslCommonTools.h"
#include "shader.h"

/**
* GLSL Shader pipeline
*/
class GLSLProgram {
public:
	GLuint _programId;
	bool _linked;

private:
	GLSLShader* _vertexShader;
	GLSLShader* _fragmentShader;

public:
	GLSLProgram();
	GLSLProgram(GLSLShader* vertex, GLSLShader* fragment);
	GLSLProgram(const char* vertexShaderFile, const char* fragmentShaderFile);
	void compile();
	void use();

private:
	void printLinkError(GLuint programId);
};
