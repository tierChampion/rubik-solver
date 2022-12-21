#pragma once

#include "loader.h"
#include <glm/gtc/random.hpp>
#include <glm/gtx/component_wise.hpp>
#include <map>

namespace splr {

	static const glm::vec3 INFINITE_CAMERA_POS = glm::vec3(0, 0, 100);

	int isCCW(const Vertex& v0, const Vertex& v1, const Vertex& v2, bool exact = false);

	struct DistancesArray {
		std::vector<uint8_t> zeros;
		std::vector<uint8_t> positives;
		std::vector<uint8_t> negatives;
	};

	class CyclicBorder {

		int _desiredWinding;
		glm::vec3 _cycleNormal;
		int _axis;

		std::vector<Vertex> _rawVerts;
		std::map<Vertex, std::vector<Vertex>> _rawEdges;
		std::vector<Vertex> _cycle;

	public:

		CyclicBorder(const glm::vec3 planeNorm);
		void appendEdge(const Vertex& v1, const Vertex& v2);
		void buildBorder(std::vector<MeshData>& halves);

		bool isEmpty() const {
			return _rawVerts.size() == 0 && _cycle.size() == 0;
		}

		int cycleSize() const {
			return _cycle.size();
		}

		Vertex operator[](int i) const {
			return _cycle[i];
		}

	private:

		void findWinding();

	};

	static const float SPLIT_PLANE_DIST = 1;
	static const int CUBIE_COUNT = 26;

	// add a border map to get the edges and the verts
	class MeshSplitter {

		std::vector<MeshData> _meshes;

		std::vector<glm::vec3> _planeNormals;

		bool _used;

	public:

		MeshSplitter(MeshData& wholeMesh) : _used(false) {
			_meshes = std::vector<MeshData>(1);
			_meshes[0] = wholeMesh;

			_planeNormals = std::vector<glm::vec3>(3);
			_planeNormals[0] = glm::vec3(1, 0, 0);
			_planeNormals[1] = glm::vec3(0, 1, 0);
			_planeNormals[2] = glm::vec3(0, 0, 1);
		}

		std::vector<MeshData>& getMeshes() {

			if (_meshes.size() != CUBIE_COUNT) {
				_meshes.resize(CUBIE_COUNT);
			}

			return _meshes;
		}

		void splitMeshIntoRubik();
		void splitSingleSide(const glm::vec3 planeNorm, int section);

	private:

		std::vector<MeshData> splitMeshAlongPlane(int meshId, const glm::vec3 planeNorm) const;

		/// 
		/// TRIVIAL MESH SPLITTING
		/// 

		Vertex getPlaneEdgeIntersection(const glm::vec3 planeNorm, const Vertex& v1, const Vertex& v2) const;

		void splitTriInHalf(int meshId, int triId, std::vector<MeshData>& halves,
			const glm::vec3 planeNorm, const DistancesArray& dists, CyclicBorder& cycle) const;

		void splitTriInThree(int meshId, int triId, std::vector<MeshData>& halves,
			const glm::vec3 planeNorm, const DistancesArray& dists, CyclicBorder& cycle) const;

		///
		/// FACE RECONSTRUCTION
		/// 

		void triangulateFace(std::vector<MeshData>& halves, const glm::vec3 planeNorm,
			CyclicBorder& cycle) const;

		///
		/// EXPERIMENTAL PART OF TRIANGULATION (STILL VERY BUGGY)
		/// 

		void earTrimming(std::vector<MeshData>& halves, std::vector<Vertex>& cyclic,
			const glm::vec3 planeNorm, int desiredWinding) const;
		bool isEar(const std::vector<Vertex>& cyclic, int earId, int planeAxis, int desiredWinding) const;
		double triArea(const glm::vec3 p0, const glm::vec3 p1, const glm::vec3 p2, int x, int y) const;
	};
}
