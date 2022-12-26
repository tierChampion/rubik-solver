#pragma once

#include "loader.h"
#include <glm/gtc/random.hpp>
#include <glm/gtx/component_wise.hpp>
#include <map>

namespace splr {

	static const glm::vec3 INFINITE_CAMERA_POS = glm::vec3(0, 0, 100);

	int isCCW(const Vertex& v0, const Vertex& v1, const Vertex& v2, bool exact = false);

	class CyclicBorder {

		int _desiredWinding;
		glm::vec3 _cycleNormal;
		int _axis;

		std::vector<Vertex> _verts;
		std::map<Vertex, std::vector<Vertex>> _edges;
		std::vector<Vertex> _cycle;

	public:

		CyclicBorder(const glm::vec3 planeNorm);
		void appendEdge(Vertex v1, Vertex v2);
		void buildBorder();

		bool isEmpty() const {
			return _verts.size() == 0 && _cycle.size() == 0;
		}

		void clearCycle() {
			_cycle.clear();
		}

		int cycleSize() const {
			return _cycle.size();
		}

		int vertexCount() const {
			return _verts.size();
		}

		glm::vec3 getNormal() const {
			return _cycleNormal;
		}

		Vertex operator[](int i) const {
			return _cycle[i];
		}

	private:

		void createEdges();
		void cleanEdges();
		void findWinding();

	};

}
