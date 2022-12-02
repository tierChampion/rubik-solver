#pragma once

#include "loader.h"

namespace splr {

	struct DistancesArray {
		std::vector<uint8_t> zeros;
		std::vector<uint8_t> positives;
		std::vector<uint8_t> negatives;
	};

	static const float SPLIT_PLANE_DIST = 1;

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
			return _meshes;
		}

		void splitMeshIntoRubik();

	private:

		std::vector<MeshData> splitMeshAlongPlane(int meshId, const glm::vec3 planeNorm) const;

		/// 
		/// TRIVIAL MESH SPLITTING
		/// 

		Vertex getPlaneEdgeIntersection(const glm::vec3 planeNorm, const Vertex& v1, const Vertex& v2) const;

		void splitTriInHalf(int meshId, int tri, std::vector<MeshData>& halves,
			std::vector<Vertex>& border, const glm::vec3 planeNorm,
			const DistancesArray& dists) const;

		void splitTriInThree(int meshId, int tri, std::vector<MeshData>& halves,
			std::vector<Vertex>& border, const glm::vec3 planeNorm,
			const DistancesArray& dists) const;

		///
		/// FACE RECONSTRUCTION
		/// 

		void triangulateFace(std::vector<MeshData>& halves,
			std::vector<Vertex>& border, const glm::vec3 planeNorm) const;

		int isCCW(const glm::vec3& p,
			const Vertex& v0, const Vertex& v1, const Vertex& v2, bool exact = false) const;

		std::vector<Vertex> buildCyclicBorder(std::vector<MeshData>& halves, std::vector<Vertex>& border,
			const glm::vec3 planeNorm, int desiredWinding) const;

		void findWinding(std::vector<Vertex>& border,
			std::vector<Vertex>& cyclic, const glm::vec3 planeNorm, int planeAxis, int desiredWinding) const;

		///
		/// EXPERIMENTAL PART OF TRIANGULATION (STILL VERY BUGGY)
		/// 

		void earTrimming(std::vector<MeshData>& halves, std::vector<Vertex>& cyclic,
			const glm::vec3 planeNorm, int desiredWinding) const;
		bool isEar(const std::vector<Vertex>& cyclic, int earId, int planeAxis, int desiredWinding) const;
		double triArea(const glm::vec3 p0, const glm::vec3 p1, const glm::vec3 p2, int x, int y) const;
	};
}
