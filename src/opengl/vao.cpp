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

	std::vector<unsigned int> vertIndices, uvIndices, normalIndices;
	std::vector<glm::vec3> tempVerts;
	std::vector<glm::vec2> tempUV;
	std::vector<glm::vec3> tempNormals;

	FILE* file;
	fopen_s(&file, path, "r");
	if (file == NULL) {
		printf("Problem while opening the file! ");
		return false;
	}

	while (1) {

		char lineHeader[128];
		int res = fscanf_s(file, "%s", lineHeader, sizeof(lineHeader));
		if (res == EOF)
			break; // Reached the end of the file

		// Vertex position
		if (strcmp(lineHeader, "v") == 0) {
			glm::vec3 vertex;
			fscanf_s(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			tempVerts.push_back(vertex);
		}
		// Vertex texture coordinates
		else if (strcmp(lineHeader, "vt") == 0) {
			glm::vec2 uv;
			fscanf_s(file, "%f %f\n", &uv.x, &uv.y);
			tempUV.push_back(uv);
		}
		// Vertex normal coordinates
		else if (strcmp(lineHeader, "vn") == 0) {
			glm::vec3 normal;
			fscanf_s(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			tempNormals.push_back(normal);
		}
		// indices
		else if (strcmp(lineHeader, "f") == 0) {

			std::string vert1, vert2, vert3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf_s(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n",
				&vertexIndex[0], &uvIndex[0], &normalIndex[0],
				&vertexIndex[1], &uvIndex[1], &normalIndex[1],
				&vertexIndex[2], &uvIndex[2], &normalIndex[2]);
			if (matches != 9) {
				printf("Problem while reading the file! \n");
				return false;
			}

			vertIndices.push_back(vertexIndex[0]);
			vertIndices.push_back(vertexIndex[1]);
			vertIndices.push_back(vertexIndex[2]);
			uvIndices.push_back(uvIndex[0]);
			uvIndices.push_back(uvIndex[1]);
			uvIndices.push_back(uvIndex[2]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);
		}
	}

	std::vector<glm::vec3> finalVerts;
	std::vector<glm::vec2> finalUVs;
	std::vector<glm::vec3> finalNormals;

	for (int i = 0; i < vertIndices.size(); i++) {
		finalVerts.push_back(tempVerts[vertIndices[i] - 1]);
		finalUVs.push_back(tempUV[uvIndices[i] - 1]);
		finalNormals.push_back(tempNormals[normalIndices[i] - 1]);

		_vertCount++;
	}

	/*
	Storage of the data for OpenGL
	*/

	GLuint vertexBuffer;
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, finalVerts.size() * sizeof(glm::vec3), &finalVerts[0], GL_STATIC_DRAW);

	GLuint uvBuffer;
	glGenBuffers(1, &uvBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
	glBufferData(GL_ARRAY_BUFFER, finalUVs.size() * sizeof(glm::vec2), &finalUVs[0], GL_STATIC_DRAW);

	GLuint normalBuffer;
	glGenBuffers(1, &normalBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
	glBufferData(GL_ARRAY_BUFFER, finalNormals.size() * sizeof(glm::vec3), &finalNormals[0], GL_STATIC_DRAW);

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