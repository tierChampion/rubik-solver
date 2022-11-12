#include "vao.h"

/**
* Stores the mesh data of a file in a Vao.
* @param objFile - mesh file to read
*/
Vao::Vao(const char* objFile) {
	_vertCount = 0;
	glGenVertexArrays(1, &_id);
	glBindVertexArray(_id);
	loadObj(objFile);
	glBindVertexArray(0);
}

/**
* Bind the Vao for rendering.
*/
void Vao::bind() {
	glBindVertexArray(_id);
	for (int a = 0; a < _attribCount; a++) {
		glEnableVertexAttribArray(a);
	}
}

/**
* Unbind the Vao after rendering.
*/
void Vao::unbind() {
	for (int a = 0; a < _attribCount; a++) {
		glDisableVertexAttribArray(a);
	}
	glBindVertexArray(0);
}

int Vao::getTriCount() {
	return _vertCount;
}

/**
* Read the data from a mesh file.
* @param path - path of the file
*/
bool Vao::loadObj(const char* path) {

	splr::MeshData data;

	bool valid = splr::loadObj(path, data);

	if (!valid) return valid;

	/*
	Storage of the data for OpenGL
	*/
	_vertCount = data.size();

	GLuint vertexBuffer;
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, _vertCount * sizeof(glm::vec3), &data.pos[0], GL_STATIC_DRAW);

	GLuint uvBuffer;
	glGenBuffers(1, &uvBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
	glBufferData(GL_ARRAY_BUFFER, _vertCount * sizeof(glm::vec2), &data.uvs[0], GL_STATIC_DRAW);

	GLuint normalBuffer;
	glGenBuffers(1, &normalBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
	glBufferData(GL_ARRAY_BUFFER, _vertCount * sizeof(glm::vec3), &data.norms[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	_attribCount = 3;

	return true;
}