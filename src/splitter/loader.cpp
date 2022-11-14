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

	std::vector<MeshData> splitMeshAlongPlane(const glm::vec3 planeNorm,
		const float planeDist, MeshData& neutralMesh) {

		MeshData positiveMesh;
		MeshData negativeMesh;

		for (int v = 0; v < neutralMesh.size(); v += 3) {
			// for each triangle
			std::vector<uint8_t> positives;
			std::vector<uint8_t> negatives;
			std::vector<uint8_t> zeros;

			for (int i = 0; i < 3; i++) {
				// plug into ax + by + cz - d
				float distance = glm::dot(neutralMesh.pos[v + i], planeNorm) - planeDist;
				if (distance > 0) positives.push_back(i);
				else if (distance < 0) negatives.push_back(i);
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

				glm::vec3 dir = neutralMesh.pos[positives[0] + v] -
					neutralMesh.pos[negatives[0] + v];

				float t = planeDist - glm::dot(planeNorm, neutralMesh.pos[negatives[0] + v]);
				t /= glm::dot(planeNorm, dir);

				// interpolate to point
				glm::vec3 p = neutralMesh.pos[negatives[0] + v] + t * dir;

				glm::vec2 uv = neutralMesh.uvs[negatives[0] + v] +
					(neutralMesh.uvs[positives[0] + v] - neutralMesh.uvs[negatives[0] + v]) *
					t / glm::length(dir);

				glm::vec3 n = neutralMesh.norms[negatives[0] + v] +
					(neutralMesh.norms[positives[0] + v] - neutralMesh.norms[negatives[0] + v]) *
					t / glm::length(dir);

				// Winding not ok depending on orientation
				// neutral -> positives -> negatives
				// else neutral -> negatives -> positives
				bool windingOrder = ((zeros[0] + 1) % 3) == positives[0];

				int start = zeros[0];

				if (windingOrder) {
					// 0 1+ p
					positiveMesh.append(neutralMesh[start + v]);
					positiveMesh.append(neutralMesh[(start + 1) % 3 + v]);
					positiveMesh.append(Vertex(p, uv, n));

					// P 2- 0
					negativeMesh.append(Vertex(p, uv, n));
					negativeMesh.append(neutralMesh[(start + 2) % 3 + v]);
					negativeMesh.append(neutralMesh[start + v]);
				}
				else {
					// 0 1+ p
					negativeMesh.append(neutralMesh[start + v]);
					negativeMesh.append(neutralMesh[(start + 1) % 3 + v]);
					negativeMesh.append(Vertex(p, uv, n));

					// P 2- 0
					positiveMesh.append(Vertex(p, uv, n));
					positiveMesh.append(neutralMesh[(start + 2) % 3 + v]);
					positiveMesh.append(neutralMesh[start + v]);
				}

				std::cout << glm::dot(dir, planeNorm) << std::endl;
			}
			else {
				// two corners are on one side and the other one is on the other side
				bool positiveBias = negatives.size() == 1;
				int startId = positiveBias ? negatives[0] : positives[0];

				// p1i = +p1 to -p0 and plane
				glm::vec3 dir = neutralMesh.pos[(startId + 1) % 3 + v] - neutralMesh.pos[startId + v];

				float t = planeDist - glm::dot(planeNorm, neutralMesh.pos[startId + v]);
				t /= glm::dot(planeNorm, dir);

				// interpolate to point
				glm::vec3 p = neutralMesh.pos[startId + v] + t * dir;

				glm::vec2 uv = neutralMesh.uvs[startId + v] +
					(neutralMesh.uvs[(startId + 1) % 3 + v] - neutralMesh.uvs[startId + v]) *
					t / glm::length(dir);

				glm::vec3 n = neutralMesh.norms[startId + v] +
					(neutralMesh.norms[(startId + 1) % 3 + v] - neutralMesh.norms[startId + v]) *
					t / glm::length(dir);

				Vertex inter1 = Vertex(p, uv, n);

				// p2i = +p2 to -p0 and plane
				dir = neutralMesh.pos[(startId + 2) % 3 + v] - neutralMesh.pos[startId + v];

				t = planeDist - glm::dot(planeNorm, neutralMesh.pos[startId + v]);
				t /= glm::dot(planeNorm, dir);

				// interpolate to point
				p = neutralMesh.pos[startId + v] + t * dir;

				uv = neutralMesh.uvs[startId + v] +
					(neutralMesh.uvs[(startId + 2) % 3 + v] - neutralMesh.uvs[startId + v]) *
					t / glm::length(dir);

				n = neutralMesh.norms[startId + v] +
					(neutralMesh.norms[(startId + 2) % 3 + v] - neutralMesh.norms[startId + v]) *
					t / glm::length(dir);

				Vertex inter2 = Vertex(p, uv, n);

				if (positiveBias) {
					// t1 = (p1i, p2i, -p0)
					negativeMesh.append(inter1);
					negativeMesh.append(inter2);
					negativeMesh.append(neutralMesh[startId + v]);
					// t2 = (+p1, +p2, p1i)
					positiveMesh.append(inter1);
					positiveMesh.append(neutralMesh[(startId + 1) % 3 + v]);
					positiveMesh.append(neutralMesh[(startId + 2) % 3 + v]);
					// t3 = (+p2, p2i, p1i)
					positiveMesh.append(neutralMesh[(startId + 2) % 3 + v]);
					positiveMesh.append(inter2);
					positiveMesh.append(inter1);
				}
				else {
					// possibly change winding order
					// t1 = (p1i, p2i, -p0)
					positiveMesh.append(inter1);
					positiveMesh.append(inter2);
					positiveMesh.append(neutralMesh[startId + v]);
					// t2 = (+p1, +p2, p1i)
					negativeMesh.append(neutralMesh[(startId + 1) % 3 + v]);
					negativeMesh.append(neutralMesh[(startId + 2) % 3 + v]);
					negativeMesh.append(inter1);
					// t3 = (+p2, p2i, p1i)
					negativeMesh.append(neutralMesh[(startId + 2) % 3 + v]);
					negativeMesh.append(inter2);
					negativeMesh.append(inter1);
				}
			}
		}

		std::vector<MeshData> ret(2);
		ret[0] = positiveMesh;
		ret[1] = negativeMesh;

		return ret;
	}
}