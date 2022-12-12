#include "meshsplitter.h"

namespace splr {

	/**
	* Calculate the shape to close the mesh cut by the plane.
	* @param posMesh - mesh on the positive side of the slicing plane
	* @param negMesh - mesh on the negative side of the slicing plane
	* @param border - list of pairs of neighbouring vertices in the border
	* @param planeNorm - normal of the slicing plane
	* @param planeDist - distance of the plane to the origin
	*/
	void MeshSplitter::triangulateFace(std::vector<MeshData>& halves,
		std::vector<Vertex>& border, const glm::vec3 planeNorm) const {

		// Winding that a convex vertex should have
		int desiredWinding = (glm::dot(glm::vec3(0, 0, 30) - planeNorm, -planeNorm) >= 0) ? 1 : 2;

		// Construct the cyclic list of the border vertices
		std::vector<Vertex> cyclic = buildCyclicBorder(halves, border, planeNorm, desiredWinding);

		// TODO for future. A more general algorithm for triangulation.
		//earTrimming(halves, cyclic, planeNorm, desiredWinding);

		// Naive triangulation (only works on convex polygons
		///*
		for (int i = 1; i < cyclic.size() - 1; i++) {

			// Forcefull winding for bugs
			int triCCW = isCCW(cyclic[0], cyclic[i], cyclic[i + 1]);

			if (triCCW == desiredWinding) {

				halves[0].append(Vertex(cyclic[0]._p, cyclic[0]._uv, planeNorm));
				halves[0].append(Vertex(cyclic[i]._p, cyclic[i]._uv, planeNorm));
				halves[0].append(Vertex(cyclic[i + 1]._p, cyclic[i + 1]._uv, planeNorm));

				halves[1].append(Vertex(cyclic[0]._p, cyclic[0]._uv, -planeNorm));
				halves[1].append(Vertex(cyclic[i + 1]._p, cyclic[i + 1]._uv, -planeNorm));
				halves[1].append(Vertex(cyclic[i]._p, cyclic[i]._uv, -planeNorm));
			}
		}
		//*/
	}

	/**
	* Determines whether the triangle formed by three vertices is wound
	* in a counter-clockwise order when viewed from a point.
	* @param p - viewpoint
	* @param v0 - Presumably most clockwise vertex
	* @param v1 - Middle vertex
	* @param v2 - Presumably most counter-clockwise vertex
	* @param exact - Have the check be exact. Off by default
	* @return whether the triangle is ccw
	*/
	int MeshSplitter::isCCW(const Vertex& v0, const Vertex& v1, const Vertex& v2, bool exact) const {

		static const glm::vec3 INFINITE_CAMERA_POS = glm::vec3(0, 0, 100);

		glm::vec3 triN = glm::cross(v1._p - v0._p,
			v2._p - v0._p);

		if (approximates(glm::vec3(0.f), triN) && !exact || triN == glm::vec3(0.f)) return 0;

		triN = glm::normalize(triN);

		int otherTest = glm::dot(INFINITE_CAMERA_POS - v0._p, triN) >= 0 ? 1 : 2;

		return otherTest;
	}

	/**
	* Create a cyclic list of the perimeter of the border.
	*
	* TODO: This doesn't work correctly if the vertex used for the winding is thin
	*
	* @param posMesh - mesh on the positive side of the slicing plane
	* @param negMesh - mesh on the negative side of the slicing plane
	* @param border - list of pairs of neighbouring vertices in the border
	* @param planeNorm - normal of the slicing plane
	* @param desiredWinding - orientation of the list. true for ccw and false for cw
	* @return list of vertices ordered in the desired winding
	*/
	std::vector<Vertex> MeshSplitter::buildCyclicBorder(std::vector<MeshData>& halves, std::vector<Vertex>& border,
		const glm::vec3 planeNorm, int desiredWinding) const {

		// Axis of the slice
		int planeAxis = (planeNorm.y != 0) * 1 + (planeNorm.z != 0) * 2;
		std::vector<Vertex> cyclic;

		// Find three vertices arranged in the correct order
		findWinding(border, cyclic, planeNorm, planeAxis, desiredWinding);

		bool fullCircle = false;

		while (!fullCircle) {

			// for the current extremities of the cyclic list, check if a vertex is equal to it.
			// than you found another edge that can you then know the orientation of.
			int i = 0;
			bool foundEdge = false;

			while (i < border.size() && !foundEdge) {

				if (border[i] == cyclic.back()) {
					foundEdge = true;

					cyclic.push_back(border[i - 2 * (i % 2) + 1]);

					border.erase(border.begin() + (i - (i % 2)),
						border.begin() + (i - (i % 2)) + 2);
				}
				i++;
			}

			fullCircle = border.size() == 0 || !foundEdge;

			// Sometimes, shape borders have gaps. Manually add all the tris
			if (!foundEdge) {
				std::cout << "yo?" << std::endl;
				for (int j = 0; j < border.size() - 1; j++) {

					int triCCW = isCCW(cyclic[0],
						border[j], border[j + 1]);

					if (triCCW == desiredWinding) {
						halves[0].append(Vertex(cyclic[0]._p, cyclic[0]._uv, planeNorm));
						halves[0].append(Vertex(border[j]._p, border[j]._uv, planeNorm));
						halves[0].append(Vertex(border[j + 1]._p, border[j + 1]._uv, planeNorm));

						halves[1].append(Vertex(cyclic[0]._p, cyclic[0]._uv, -planeNorm));
						halves[1].append(Vertex(border[j + 1]._p, border[j + 1]._uv, -planeNorm));
						halves[1].append(Vertex(border[j]._p, border[j]._uv, -planeNorm));

					}
					else {
						halves[0].append(Vertex(cyclic[0]._p, cyclic[0]._uv, planeNorm));
						halves[0].append(Vertex(border[j + 1]._p, border[j + 1]._uv, planeNorm));
						halves[0].append(Vertex(border[j]._p, border[j]._uv, planeNorm));

						halves[1].append(Vertex(border[j]._p, border[j]._uv, -planeNorm));
						halves[1].append(Vertex(border[j + 1]._p, border[j + 1]._uv, -planeNorm));
						halves[1].append(Vertex(cyclic[0]._p, cyclic[0]._uv, -planeNorm));
					}
				}
			}
		}

		return cyclic;
	}

	/*
	* TODO: There is a known bug where found convex vertex forms a thin triangle,
	* with which the winding can't be determined.
	*
	* Possible fix: Generate a random direction and check if there is a unique maximal vert.
	* Otherwise, retry with another random direction. It seems to work for most cases, but it gets very slow.
	*/
	void MeshSplitter::findWinding(std::vector<Vertex>& border,
		std::vector<Vertex>& cyclic, const glm::vec3 planeNorm, int planeAxis, int desiredWinding) const {

		int x = (planeAxis + 1) % 3;
		int y = (planeAxis + 2) % 3;

		int convex = 0;

		float max = 0;

		bool foundConvex = false;

		float maxX = border[0]._p[x];
		float maxY = border[0]._p[y];

		do {

			float xStr = glm::gaussRand(0.0f, 1.0f);
			float yStr = glm::gaussRand(0.0f, 1.0f);

			int convex = 0;

			float max = border[0]._p[x] * xStr + border[0]._p[y] * yStr;

			// Find the vertex which maximises the two dimensions, it is necessarly convex
			for (int i = 1; i < border.size(); i++) {

				if (border[i] == border[convex]) continue;

				float dist = border[i]._p[x] * xStr + border[i]._p[y] * yStr;

				if (dist > max) {
					convex = i;
					max = dist;
				}
			}

			int neighbour = convex - 2 * (convex % 2) + 1;

			Vertex vMiddle = border[convex];
			Vertex vExtrem = border[neighbour];

			for (int i = convex; i < border.size() && cyclic.size() == 0; i++) {

				bool isZero = (border[i] == border[convex]) && i != convex;

				if (isZero) {

					int triCCW = isCCW(vExtrem,
						vMiddle, border[i - 2 * (i % 2) + 1], true);

					if (triCCW == 0) {
						break;
					}

					if (triCCW == desiredWinding) {

						cyclic.push_back(vExtrem);
						cyclic.push_back(vMiddle);
						cyclic.push_back(border[i - 2 * (i % 2) + 1]);
					}
					else {
						cyclic.push_back(border[i - 2 * (i % 2) + 1]);
						cyclic.push_back(vMiddle);
						cyclic.push_back(vExtrem);
					}

					// Remove the tris that were processed

					border.erase(border.begin() + i - (i % 2),
						border.begin() + i - (i % 2) + 2);
					border.erase(border.begin() + std::min(convex, neighbour),
						border.begin() + std::min(convex, neighbour) + 2);

					foundConvex = true;

				}
			}
		} while (!foundConvex);
	}

	///
	/// TODO: EXPERIMENTAL FUNCTIONALITIES FROM THIS POINT
	/// WORKS FOR ALMOST EVERYTHING, BUT CONCAVITIES STILL HAVE TROUBLES
	///

	/**
	* Calculate the area of the triangle with the given vertices
	* @param x - index of the x dimension to use
	* @param y - index of the y dimension to use
	*/
	double MeshSplitter::triArea(const glm::vec3 p0,
		const glm::vec3 p1, const glm::vec3 p2, int x, int y) const {

		return (std::fabs(p0[x] * (p2[y] - p1[y]) +
			p1[x] * (p2[y] - p0[y]) +
			p2[x] * (p1[y] - p0[y])));
	}

	/**
	* Determine whether the vertex constitutes an ear.
	* An ear is a convex vertex where the triangle with its neighbouring
	* vertices contains no other points.
	* @param cyclic - Ordered list of the vertices
	* @param earId - index of the ear to check
	* @param desiredWinding - orientation of a tri with a concave vertex
	*/
	bool MeshSplitter::isEar(const std::vector<Vertex>& cyclic,
		int earId, int plane, int desiredWinding) const {

		int vPrev = (earId + cyclic.size() - 1) % cyclic.size();
		int vNext = (earId + 1) % cyclic.size();

		int x = (plane + 1) % 3;
		int y = (x + 1) % 3;

		// TODO With very thin tris, the area is almost 0 and the cross product is very small
		// this makes determining if it is an ear very difficult

		double insideArea = triArea(cyclic[vPrev]._p, cyclic[earId]._p, cyclic[vNext]._p, x, y);

		if (insideArea == 0) {
			return true;
		}

		int isConvex = isCCW(cyclic[vPrev], cyclic[earId], cyclic[vNext]);

		//if (isConvex == 0)
			//return true;

		if (isConvex != desiredWinding)
			return false;

		// TODO //

		// determine the used dimensions
		// Calculate the total area of the ear triangle

		// Calculate the sum of the three small triangles for each vertices.
		// If the sum is equal to the total, the point is inside the triangle
		// thus, we don't have an ear

		for (int i = 0; i < cyclic.size() - 3; i++) {

			int index = (i + vNext) % cyclic.size();

			// Calculate the sum of the three small triangles for each vertices.
			double area1 = triArea(cyclic[index]._p, cyclic[earId]._p, cyclic[vNext]._p, x, y),
				area2 = triArea(cyclic[vPrev]._p, cyclic[index]._p, cyclic[vNext]._p, x, y),
				area3 = triArea(cyclic[vPrev]._p, cyclic[earId]._p, cyclic[index]._p, x, y);

			// If the sum is equal to the total, the point is inside the triangle
			// thus, we don't have an ear
			if (area1 + area2 + area3 == insideArea) return false;
		}

		// TODO //

		return true;
	}

	/**
	* TODO works pretty well except that it doesnt really rebuild a concave border.
	* Test with the pyramid
	*/
	void MeshSplitter::earTrimming(std::vector<MeshData>& halves, std::vector<Vertex>& cyclic,
		const glm::vec3 planeNorm, int desiredWinding) const {

		// TODO //
		// Is this corner an ear?
		//	- Convex corner (orientation of the tri is not the desired orientation)
		//	- No vertices are inside the triangle (look at area of small tris)
		// Form a tri with the ear and remove the ear.
		//

		// Make it does what it needs to do.
		int planeAxis = (planeNorm.y != 0) * 1 + (planeNorm.z != 0) * 2;

		// square pyramid, stuff are missing
		int triCount = 11;
		int counter = 0;

		while (cyclic.size() >= 3) {

			bool foundEar = false;
			int i = 0;

			while (!foundEar && i < cyclic.size()) {
				// doesnt work correctly
				if (isEar(cyclic, i, planeAxis, desiredWinding)) {

					foundEar = true;

					Vertex vFront(cyclic[(i + cyclic.size() - 1) % cyclic.size()]._p,
						cyclic[(i + cyclic.size() - 1) % cyclic.size()]._uv,
						planeNorm);

					Vertex vBack(cyclic[(i + 1) % cyclic.size()]._p,
						cyclic[(i + 1) % cyclic.size()]._uv,
						planeNorm);

					// add to meshes (TODO fix normals)

					halves[0].append(vFront);
					halves[0].append(Vertex(cyclic[i]._p, cyclic[i]._uv, planeNorm));
					halves[0].append(vBack);

					halves[1].append(vBack);
					halves[1].append(Vertex(cyclic[i]._p, cyclic[i]._uv, planeNorm));
					halves[1].append(vFront);

					// remove from list
					cyclic.erase(cyclic.begin() + i, cyclic.begin() + i + 1);

					counter++;

					if (counter >= triCount) {
						cyclic.clear();
					}
				}

				i++;
			}

			// add last tri (NORMALY THIS SHOULD NOT BE CONSIDERED)

			if (!foundEar && cyclic.size() >= 3) {

				int triCCW = isCCW(cyclic[0],
					cyclic[2], cyclic[1]); // point faces camera

				std::cout << "No ears: " << cyclic.size() << " " << (triCCW == desiredWinding) << std::endl;
				//break;
				halves[0].append(Vertex(cyclic[0]._p, cyclic[0]._uv, planeNorm));
				halves[0].append(Vertex(cyclic[2]._p, cyclic[2]._uv, planeNorm));
				halves[0].append(Vertex(cyclic[1]._p, cyclic[1]._uv, planeNorm));

				halves[1].append(Vertex(cyclic[0]._p, cyclic[0]._uv, -planeNorm));
				halves[1].append(Vertex(cyclic[1]._p, cyclic[1]._uv, -planeNorm));
				halves[1].append(Vertex(cyclic[2]._p, cyclic[2]._uv, -planeNorm));

				cyclic.erase(cyclic.begin() + 1, cyclic.begin() + 2);
			}
		}
	}
}