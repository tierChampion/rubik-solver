#pragma once

#include "../splitter/loader.h"
#include <glew.h>

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

