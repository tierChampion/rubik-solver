#pragma once

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <glm/glm.hpp>
#include <vector>
#include <iostream>
#include <string>

namespace splr {

	struct Vertex {

		glm::vec3 p;
		glm::vec2 uv;
		glm::vec3 n;

		Vertex(const glm::vec3 p, const glm::vec2 uv, const glm::vec3 n) : p(p), uv(uv), n(n) {}

	};

	struct MeshData {

		std::vector<glm::vec3> pos;
		std::vector<glm::vec2> uvs;
		std::vector<glm::vec3> norms;

	public:

		MeshData() {}

		void append(const Vertex vert) {
			pos.push_back(vert.p);
			uvs.push_back(vert.uv);
			norms.push_back(vert.n);
		}

		Vertex operator[](int i) const {
			return Vertex(pos[i], uvs[i], norms[i]);
		}

		int size() const {
			return pos.size();
		}
	};

	bool loadObj(const char* path, MeshData& finalMesh);

	std::vector<MeshData> splitMeshAlongPlane(const glm::vec3 planeNorm, const float planeDist,
		MeshData& neutralMesh);
}