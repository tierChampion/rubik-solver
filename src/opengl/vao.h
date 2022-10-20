#pragma once

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <glew.h>
#include <vector>
#include <string>

class Vao {

	GLuint id;
	GLuint attribCount;
	GLuint vertCount;

public:
	Vao(const char* objFile);
	void bind();
	int getTriCount();
	void unbind();

private:
	bool loadObj(const char* path);

};

