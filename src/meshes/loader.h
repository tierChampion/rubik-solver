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

namespace splr {

	inline bool isNearZero(const glm::vec3& v) {
		static const float delta = 0.002f;

		return std::fabsf(glm::compMax(v)) < delta &&
			std::fabsf(glm::compMin(v)) < delta;
	}

	/**
	* Spaces out the bits of number by three.
	* @param x - number to space out.
	*/
	inline uint32_t leftShift3(uint32_t x) {

		if (x == (1 << 10)) --x;
		x = (x | (x << 16)) & 0b00000011000000000000000011111111;
		x = (x | (x << 8)) & 0b00000011000000001111000000001111;
		x = (x | (x << 4)) & 0b00000011000011000011000011000011;
		x = (x | (x << 2)) & 0b00001001001001001001001001001001;
		return x;
	}

	/*
	* Encode a 3D position into a 30 bit morton code.
	* @param v - 3D position to encode
	*/
	inline uint32_t encodeToMorton(const glm::vec3& v) {
		return leftShift3(v.x) | (leftShift3(v.y) << 1) | (leftShift3(v.z) << 2);
	}

	struct Vertex {

		glm::vec3 _p;
		glm::vec2 _uv;
		glm::vec3 _n;
		uint32_t _morton;

		Vertex(const glm::vec3 p, const glm::vec2 uv, const glm::vec3 n) : _p(p), _uv(uv), _n(n) {

			glm::vec3 offsetedP = _p + glm::vec3(10, 10, 10);
			_morton = encodeToMorton(offsetedP * (float)(1 << 8));
		}

		bool operator==(const Vertex& v) const {

			return isNearZero(this->_p - v._p);

			// ERROR, doesn't work properly since
			//uint32_t delt = this->_morton - v._morton;
			//uint32_t invDelt = v._morton - this->_morton;

			//uint32_t delta = this->_morton ^ v._morton;

			//return delta <= 0b111;

			//return delt <= 0b1 || invDelt <= 0b1;
		}

		bool operator!=(const Vertex& v) const {
			return !(*this == v);
		}

		bool operator<(const Vertex& v) const {

			if (*this == v) return false;

			return this->_morton < v._morton;
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