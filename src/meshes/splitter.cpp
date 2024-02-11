#include "meshes/meshsplitter.h"

namespace splr {

	/**
	* Cuts the mesh into the 27 cubies of the Rubik's cube.
	*/
	void MeshSplitter::splitMeshIntoRubik() {

		for (int i = 0; i < _planeNormals.size(); i++) {

			int size = _meshes.size();

			for (int j = 0; j < size; j++) {

				// Split with the positive plane
				std::vector<splr::MeshData> split = splitMeshAlongPlane(0, -_planeNormals[i]);

				_meshes.push_back(split[0]);
				_meshes.push_back(split[1]);

				_meshes.erase(_meshes.begin());

				// Split the negative part with the negative plane
				splr::MeshData negative = _meshes.back();

				split = splitMeshAlongPlane(_meshes.size() - 1, _planeNormals[i]);

				_meshes.pop_back();

				_meshes.push_back(split[1]);
				_meshes.push_back(split[0]);
			}
		}
	}

	/**
	* For debugging purposses, do a single cut of the mesh.
	* @param planeNorm - Normal of the slicing plane
	* @param section - Half to keep. 0 for positive mesh and 1 for negative mesh.
	*/
	void MeshSplitter::splitSingleSide(const glm::vec3 planeNorm, int section) {

		std::vector<splr::MeshData> split = splitMeshAlongPlane(0, planeNorm);

		_meshes.push_back(split[section == 0 ? 0 : 1]);

		_meshes.erase(_meshes.begin());
	}

	/**
	* Seperates a mesh into two meshes with a plane.
	* @param planeNorm - surface normal of the plane
	* @param planeDist - distance of the plane from the origin
	*/
	std::vector<MeshData> MeshSplitter::splitMeshAlongPlane(int meshId, const glm::vec3 planeNorm) const {

		MeshData original = _meshes[meshId];

		std::vector<MeshData> halves = std::vector<MeshData>(2);
		halves[0] = MeshData();
		halves[1] = MeshData();

		// TODO seems to be ok with morton codes
		CyclicBorder cycle(planeNorm);

		for (int v = 0; v < original.size(); v += 3) {
			// for each triangle
			DistancesArray dists;

			for (int i = 0; i < 3; i++) {
				// plug into ax + by + cz - d
				float distance = glm::dot(original._pos[v + i], planeNorm) - SPLIT_PLANE_DIST;
				if (distance > 0) dists.positives.push_back(i);
				else if (distance < 0) dists.negatives.push_back(i);
				else dists.zeros.push_back(i);
			}

			if (dists.negatives.size() == 0) {
				// add to positive mesh (no negative corners)

				for (int i = 0; i < 3; i++) {
					halves[0].append(original[v + i]);
				}
			}
			else if (dists.positives.size() == 0) {
				// add to negative mesh (no positive corners)

				for (int i = 0; i < 3; i++) {
					halves[1].append(original[v + i]);
				}
			}
			else if (dists.positives.size() == dists.negatives.size()) {
				// one corner on the plane and the other two on each sides

				splitTriInHalf(meshId, v, halves,
					planeNorm, dists, cycle);
			}
			else {
				// two corners are on one side and the other one is on the other side
				splitTriInThree(meshId, v, halves,
					planeNorm, dists, cycle);
			}

			// Directly add triangle edge
			if (dists.zeros.size() == 2) {
				cycle.appendEdge(original[v + dists.zeros[0]], original[v + dists.zeros[1]]);
			}
		}
		// ear trimming to triangulate the face
		if (!cycle.isEmpty()) {

			triangulateFace(halves, cycle);
		}

		return halves;
	}

	/**
	* Get the intersection between the slicing plane and an edge.
	* @param planeNorm - Normal of the slicing plane
	* @param v1 - First vertex of the edge
	* @param v2 - Second vertex of the edge
	* @return Vertex between v1 and v2 that lies on the plane
	*/
	Vertex MeshSplitter::getPlaneEdgeIntersection(const glm::vec3 planeNorm,
		const Vertex& v1, const Vertex& v2) const {

		glm::vec3 dir = v2._p - v1._p;

		float t = SPLIT_PLANE_DIST - glm::dot(planeNorm, v1._p);
		t /= glm::dot(planeNorm, dir);

		// interpolate to point
		glm::vec3 p = v1._p + t * dir;

		glm::vec2 uv = v1._uv + (v2._uv - v1._uv) * t / glm::length(dir);

		glm::vec3 n = v1._n + (v2._n - v1._n) * t / glm::length(dir);

		return Vertex(p, uv, n);
	}

	/**
	* Slice a triangle that is split right down the middle.
	* In other words, the plane intersects with one edge and one vertex of
	* the triangle.
	* @param meshId - Index of the current mesh
	* @param triId - Index of the current triangle
	* @param halves - Positive and negative parts of the mesh
	* @param planeNorm - Normal of the slicing plane
	* @param dists - Signs of the three vertices of the triangle
	* @param cycle - Cyclic list of the vertices of the intersection polygon of the mesh
	*/
	void MeshSplitter::splitTriInHalf(int meshId, int triId, std::vector<MeshData>& halves,
		const glm::vec3 planeNorm, const DistancesArray& dists, CyclicBorder& cycle) const {

		MeshData mesh = _meshes[meshId];

		Vertex intersection = getPlaneEdgeIntersection(planeNorm,
			mesh[dists.negatives[0] + triId], mesh[dists.positives[0] + triId]);

		// Order to wrap the triangles
		bool windingOrder = ((dists.zeros[0] + 1) % 3) == dists.positives[0];

		int start = dists.zeros[0];

		if (windingOrder) {
			halves[0].append(mesh[start + triId]);
			halves[0].append(mesh[(start + 1) % 3 + triId]);
			halves[0].append(intersection);

			halves[1].append(intersection);
			halves[1].append(mesh[(start + 2) % 3 + triId]);
			halves[1].append(mesh[start + triId]);
		}
		else {
			halves[1].append(mesh[start + triId]);
			halves[1].append(mesh[(start + 1) % 3 + triId]);
			halves[1].append(intersection);

			halves[0].append(intersection);
			halves[0].append(mesh[(start + 2) % 3 + triId]);
			halves[0].append(mesh[start + triId]);
		}

		/*
		* When the triangle is very slim and there is only one intersection.
		*/
		if (intersection != mesh[start + triId]) {

			cycle.appendEdge(intersection, mesh[start + triId]);
		}
	}

	/**
	* Slice a triangle that is split on a bias.
	* In other words, the plane intersects with two edges of
	* the triangle.
	* @param meshId - Index of the current mesh
	* @param triId - Index of the current triangle
	* @param halves - Positive and negative parts of the mesh
	* @param planeNorm - Normal of the slicing plane
	* @param dists - Signs of the three vertices of the triangle
	* @param cycle - Cyclic list of the vertices of the intersection polygon of the mesh
	*/
	void MeshSplitter::splitTriInThree(int meshId, int triId, std::vector<MeshData>& halves,
		const glm::vec3 planeNorm, const DistancesArray& dists, CyclicBorder& cycle) const {

		MeshData mesh = _meshes[meshId];

		bool positiveBias = dists.negatives.size() == 1;
		int startId = positiveBias ? dists.negatives[0] : dists.positives[0];

		Vertex inter1 = getPlaneEdgeIntersection(planeNorm,
			mesh[startId + triId], mesh[(startId + 1) % 3 + triId]);

		Vertex inter2 = getPlaneEdgeIntersection(planeNorm,
			mesh[startId + triId], mesh[(startId + 2) % 3 + triId]);

		if (positiveBias) {
			halves[1].append(inter1);
			halves[1].append(inter2);
			halves[1].append(mesh[startId + triId]);

			halves[0].append(inter1);
			halves[0].append(mesh[(startId + 1) % 3 + triId]);
			halves[0].append(mesh[(startId + 2) % 3 + triId]);

			halves[0].append(mesh[(startId + 2) % 3 + triId]);
			halves[0].append(inter2);
			halves[0].append(inter1);
		}
		else {
			halves[0].append(inter1);
			halves[0].append(inter2);
			halves[0].append(mesh[startId + triId]);

			halves[1].append(inter1);
			halves[1].append(mesh[(startId + 1) % 3 + triId]);
			halves[1].append(mesh[(startId + 2) % 3 + triId]);

			halves[1].append(mesh[(startId + 2) % 3 + triId]);
			halves[1].append(inter2);
			halves[1].append(inter1);
		}

		/*
		* When the triangle is very slim and there is only one intersection.
		*/
		if (inter1 != inter2) {

			cycle.appendEdge(inter1, inter2);
		}
	}
}