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
	Vao(const splr::MeshData& mesh);
	void bind() const;
	int getTriCount() const;
	void unbind() const;

private:
	bool loadMesh(const splr::MeshData& mesh);

};

