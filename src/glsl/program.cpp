#include "glsl/program.h"

GLSLProgram::GLSLProgram() :
	_programId(0), _vertexShader(NULL), _fragmentShader(NULL), _linked(false) {}

GLSLProgram::GLSLProgram(GLSLShader* vertex, GLSLShader* fragment) :
	_programId(0), _vertexShader(vertex), _fragmentShader(fragment), _linked(false) {}

GLSLProgram::GLSLProgram(const char* vertexShaderFile, const char* fragmentShaderFile) :
	_programId(0), _linked(false) {

	_vertexShader = new GLSLShader("Vertex Shader", loadFileToString(vertexShaderFile), GL_VERTEX_SHADER);
	_fragmentShader = new GLSLShader("Fragment Shader", loadFileToString(fragmentShaderFile), GL_FRAGMENT_SHADER);
}

/**
* Compile the shader program and its shaders
*/
void GLSLProgram::compile() {
	/* Create empty shader and attach shaders. */
	_programId = glCreateProgram();
	GLSLShader* shaders[2] = { _vertexShader, _fragmentShader };
	for (unsigned int i = 0; i < 2; i++) {
		if (shaders[i] != NULL) {
			if (!shaders[i]->_compiled) { shaders[i]->compile(); }
			if (shaders[i]->_compiled) {
				glAttachShader(_programId, shaders[i]->_shaderId);
				printf("(P) Attached shader \"%s\"(%i) to program (%i)\n", shaders[i]->_shaderName.c_str(),
					shaders[i]->_shaderId, _programId);
			}
			else {
				printf("(P) Failed to attache shader \"%s\"(%i) to program (%i)\n", shaders[i]->_shaderName.c_str(),
					shaders[i]->_shaderId, _programId);
				glDeleteProgram(_programId);
				return;
			}
		}
	}
	/* Link program. */
	glLinkProgram(_programId);
	GLint isLinked = 0;
	/* Error check. */
	glGetProgramiv(_programId, GL_LINK_STATUS, &isLinked);
	if (isLinked == GL_FALSE) {
		printLinkError(_programId);
		glDeleteProgram(_programId);
		_linked = false;
	}
	else {
		_linked = true;
		printf("(P) Linked program %i \n", _programId);
	}
}

/**
* Bind the program for use
*/
void GLSLProgram::use() {
	glUseProgram(this->_programId);
}

/**
* Show the possible shader linking errors
*/
void GLSLProgram::printLinkError(GLuint programId) {
	GLint infoLogLength = 0;
	glGetProgramiv(programId, GL_INFO_LOG_LENGTH, (GLint*)&infoLogLength);
	char* infoLog = (char*)malloc(infoLogLength);
	glGetProgramInfoLog(programId, infoLogLength, NULL, infoLog);
	printf("(P) Program compilation error: %s\n", infoLog);
	free(infoLog);
}
