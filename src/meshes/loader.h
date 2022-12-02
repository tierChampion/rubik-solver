#pragma once

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <glm/glm.hpp>
#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>
#include <iostream>
#include <string>
#include <algorithm>

namespace splr {

	inline bool approximates(const glm::vec3& v1, const glm::vec3& v2) {
		static const float delta = 0.002f;

		glm::vec3 diff = v1 - v2;

		return std::abs(diff.x) < delta &&
			std::abs(diff.y) < delta &&
			std::abs(diff.z) < delta;
	}

	struct Vertex {

		glm::vec3 p;
		glm::vec2 uv;
		glm::vec3 n;

		Vertex(const glm::vec3 p, const glm::vec2 uv, const glm::vec3 n) : p(p), uv(uv), n(n) {}

		bool operator==(const Vertex& v) const {

			return approximates(this->p, v.p);
		}

		bool operator!=(const Vertex& v) const {
			return !(*this == v);
		}
	};

	struct MeshData {

		std::vector<glm::vec3> pos;
		std::vector<glm::vec2> uvs;
		std::vector<glm::vec3> norms;

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
}