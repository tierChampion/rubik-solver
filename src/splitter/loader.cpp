#include "loader.h"

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

		for (int i = 0; i < vertIndices.size(); i++) {

			mesh.append(Vertex(
				tempVerts[vertIndices[i] - 1],
				tempUV[uvIndices[i] - 1],
				tempNormals[normalIndices[i] - 1]
			));
		}
	}

	void splitMeshAlongPlane(MeshData& neutralMesh) {

		MeshData positiveMesh;
		MeshData negativeMesh;

		glm::vec3 planeNorm(0, 1, 0);
		float planeDist = 1; // +- 1 since the cubie is of size 2

		for (int v = 0; v < neutralMesh.size() / 3; v += 3) {
			// for each triangle
			std::vector<uint8_t> positives;
			std::vector<uint8_t> negatives;
			std::vector<uint8_t> zeros;

			for (int i = 0; i < 3; i++) {
				// plug into ax + by + cz + d
				float distance = 0;
				if (distance > 0) positives.push_back(i);
				if (distance < 0) negatives.push_back(i);
				else zeros.push_back(i);
			}

			if (negatives.size() == 0) {
				// add to positive mesh (no negative corners)
				for (int i = 0; i < 3; i++) {
					positiveMesh.append(neutralMesh[v + i]);
				}
			}
			else if (positives.size() == 0) {
				// add to negative mesh (no positive corners)
				for (int i = 0; i < 3; i++) {
					negativeMesh.append(neutralMesh[v + i]);
				}
			}
			else if (positives.size() == negatives.size()) {
				// one corner on the plane and the other two on each sides
				// (each indicators has only one bit set and is a power of two)

				glm::vec3 dir = neutralMesh.pos[positives[0]] - neutralMesh.pos[negatives[0]];

				float t = planeDist - glm::dot(planeNorm, neutralMesh.pos[negatives[0]]);
				t /= glm::dot(planeNorm, dir);

				// interpolate to point
				glm::vec3 p = neutralMesh.pos[negatives[0]] + t * dir;

				glm::vec2 uv = neutralMesh.uvs[negatives[0]] +
					(neutralMesh.uvs[positives[0]] - neutralMesh.uvs[negatives[0]]) *
					t / glm::length(dir);

				glm::vec3 n = neutralMesh.norms[negatives[0]] +
					(neutralMesh.norms[positives[0]] - neutralMesh.norms[negatives[0]]) *
					t / glm::length(dir);

				// POSSIBLE ERROR, WINDING IS NOT OK
				positiveMesh.append(Vertex(p, uv, n));
				positiveMesh.append(neutralMesh[positives[0]]);
				positiveMesh.append(neutralMesh[zeros[0]]);

				negativeMesh.append(Vertex(p, uv, n));
				negativeMesh.append(neutralMesh[zeros[0]]);
				negativeMesh.append(neutralMesh[negatives[0]]);

			}
			else {
				// two corners are on one side and the other one is on the other side

				int startId = positives.size() == 1 ? positives[0] : negatives[0];

				// p1i = +p1 to -p0 and plane
				// p2i = +p2 to -p0 and plane

				// t1 = (p1i, p2i, -p0)
				// t2 = (+p1, +p2, p1i)
				// t3 = (+p2, p2i, p1i)
			}
		}
	}
}