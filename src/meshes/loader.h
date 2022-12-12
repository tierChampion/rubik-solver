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

		glm::vec3 _p;
		glm::vec2 _uv;
		glm::vec3 _n;

		Vertex(const glm::vec3 p, const glm::vec2 uv, const glm::vec3 n) : _p(p), _uv(uv), _n(n) {}

		bool operator==(const Vertex& v) const {

			return approximates(this->_p, v._p);
		}

		bool operator!=(const Vertex& v) const {
			return !(*this == v);
		}
	};

	struct MeshData {

		std::vector<glm::vec3> _pos;
		std::vector<glm::vec2> _uvs;
		std::vector<glm::vec3> _norms;

		MeshData() {}

		void append(const Vertex vert) {
			_pos.push_back(vert._p);
			_uvs.push_back(vert._uv);
			_norms.push_back(vert._n);
		}

		Vertex operator[](int i) const {
			return Vertex(_pos[i], _uvs[i], _norms[i]);
		}

		int size() const {
			return _pos.size();
		}
	};

	bool loadObj(const char* path, MeshData& finalMesh);
}