#pragma once

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/component_wise.hpp>
#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>
#include <iostream>
#include <string>
#include <algorithm>
#include <cstring>

namespace splr {

	inline bool isNearZero(const glm::vec3& v) {
		// precision
		static const float delta = 0.0001f;

		return std::fabs(glm::compMax(v)) < delta &&
			std::fabs(glm::compMin(v)) < delta;

	}

	struct Vertex {

		glm::vec3 _p;
		glm::vec2 _uv;
		glm::vec3 _n;
		uint32_t _id = -1;

		Vertex(const glm::vec3 p, const glm::vec2 uv, const glm::vec3 n) : _p(p), _uv(uv), _n(n) {}

		void setId(uint32_t mort) {
			_id = mort;
		}

		bool operator==(const Vertex& v) const {

			return isNearZero(this->_p - v._p);
		}

		bool operator!=(const Vertex& v) const {
			return !(*this == v);
		}

		bool operator<(const Vertex& v) const {

			if (*this == v) {
				return false;
			}

			return this->_id < v._id;
		}

		bool exactlyEquals(const Vertex& v) const {
			return this->_p == v._p;
		}

		void print() const {
			std::cout << _p[0] << " " << _p[1] << " " << _p[2] << std::endl;
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