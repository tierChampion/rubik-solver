#pragma once

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <glm/glm.hpp>
#include <vector>
#include <iostream>
#include <string>
#include <algorithm>

namespace splr {

	inline bool approximates(const glm::vec3& v1, const glm::vec3& v2) {
		static const float delta = 0.001f;

		glm::vec3 diff = v1 - v2;

		return std::abs(diff.x) < delta &&
			std::abs(diff.y) < delta &&
			std::abs(diff.z) < delta;
	}

	inline bool roughlyApproximates(const glm::vec3& v1, const glm::vec3& v2) {
		static const float roughDelta = 0.1f;

		glm::vec3 diff = v1 - v2;

		return std::abs(diff.x) < roughDelta &&
			std::abs(diff.y) < roughDelta &&
			std::abs(diff.z) < roughDelta;
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

		std::vector<MeshData> splitMeshAlongPlane(const glm::vec3 planeNorm, const float planeDist) const;

	private:

		void splitTriInHalf(MeshData& posMesh, MeshData& negMesh,
			std::vector<Vertex>& border, const glm::vec3 planeNorm, float planeDist,
			int v, const std::vector<uint8_t>& positives, const std::vector<uint8_t>& negatives,
			const std::vector<uint8_t>& zeros) const;

		void splitTriInThree(MeshData& posMesh, MeshData& negMesh,
			std::vector<Vertex>& border, const glm::vec3 planeNorm, float planeDist,
			int v, const std::vector<uint8_t>& positives, const std::vector<uint8_t>& negatives,
			const std::vector<uint8_t>& zeros) const;

		void recreateFace(MeshData& posMesh, MeshData& negMesh,
			std::vector<Vertex>& border, const glm::vec3 planeNorm, float planeDist) const;

		std::vector<Vertex> buildCyclicBorder(MeshData& posMesh, MeshData& negMesh, std::vector<Vertex>& border,
			const glm::vec3& planeNorm, float planeDist) const;

	};

	bool loadObj(const char* path, MeshData& finalMesh);

	bool isCCW(const glm::vec3& p, const Vertex& v0, const Vertex& v1, const Vertex& v2);
}