#include "meshes/loader.h"

/// 
/// TODO: Reformat this file for readability and logic
/// Separate the relevent parts into their own file since the process is incredibly long
/// And confusing. 
/// 

namespace splr {

	/**
	* Load the data inside of an obj file into vectors of vertices.
	* @param path - location of the file
	* @param finalVerts - Storage for the vertex position
	* @param finalUVs - Storage for the vertex 2D coordinates
	* @param finalNormals - Storage for the vertex normals
	* @return if the reading of the file was succesfull
	*/
	bool loadObj(const char* path, MeshData& mesh) {

		std::vector<unsigned int> vertIndices, uvIndices, normalIndices;
		std::vector<glm::vec3> tempVerts;
		std::vector<glm::vec2> tempUV;
		std::vector<glm::vec3> tempNormals;

		FILE* file;
		fopen_s(&file, path, "r");
		if (file == NULL) {
			std::cerr << "ERROR: The given OBJ file cannot be found." << std::endl;
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
					std::cerr << "ERROR: Check if the UVs and the Normals "
						<< "of the mesh are defined in the OBJ file." << std::endl;
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

		for (int i = 0; i < vertIndices.size(); i++) {

			mesh.append(Vertex(
				tempVerts[vertIndices[i] - 1],
				tempUV[uvIndices[i] - 1],
				tempNormals[normalIndices[i] - 1]
			));
		}

		return true;
	}
}