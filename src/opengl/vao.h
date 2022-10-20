#pragma once

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <glew.h>
#include <vector>
#include <string>

/**
* Vertex array object. Stores mesh data.
*/
class Vao {

	GLuint _id;
	GLuint _attribCount;
	GLuint _vertCount;

public:
	Vao(const char* objFile);
	void bind();
	int getTriCount();
	void unbind();

private:
	bool loadObj(const char* path);

};

