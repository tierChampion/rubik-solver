#pragma once

#include "loader.h"
#include "cyclic.h"

namespace splr {

	struct DistancesArray {
		std::vector<uint8_t> zeros;
		std::vector<uint8_t> positives;
		std::vector<uint8_t> negatives;
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

		void triangulateFace(std::vector<MeshData>& halves, CyclicBorder& cycle) const;

		///
		/// EXPERIMENTAL PART OF TRIANGULATION (STILL VERY BUGGY)
		/// 

		void earTrimming(std::vector<MeshData>& halves, CyclicBorder& cycle) const;
		bool isEar(CyclicBorder& cycle, int earId) const;
		double triArea(const glm::vec3 p0, const glm::vec3 p1, const glm::vec3 p2, int x, int y) const;
	};
}
