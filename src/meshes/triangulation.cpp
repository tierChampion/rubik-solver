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
	void MeshSplitter::triangulateFace(std::vector<MeshData>& halves, CyclicBorder& cycle) const {

		// Repeat for all polygons
		do {

			// Build the cycle of the perimeter
			cycle.buildBorder();

			// Triangulate the polygon
			earTrimming(halves, cycle);

			cycle.clearCycle();

		} while (cycle.edgeCount() > 2);
	}

	/**
	* Triangulation done with fanning triangles.
	* All triangle are formed with one vertex and one edge,
	* where the vertex is the same for all triangles.
	* Only works correctly with convex polygons, creates excess triangles with concave triangles.
	* @param halves - The positive and negatives halves of the mesh to split
	* @param cycle - Cyclic perimeter of the polygon to triangulate
	*/
	void MeshSplitter::naiveTriangulation(std::vector<MeshData>& halves, CyclicBorder& cycle) const {

		glm::vec3 normal = cycle.getNormal();

		for (int i = 1; i < cycle.cycleSize() - 1; i++) {

			int triCCW = isCCW(cycle[0], cycle[i], cycle[i + 1]);

			// Thin triangles don't need to be rendered.
			if (triCCW != 0) {

				halves[0].append(Vertex(cycle[0]._p, cycle[0]._uv, normal));
				halves[0].append(Vertex(cycle[i]._p, cycle[i]._uv, normal));
				halves[0].append(Vertex(cycle[i + 1]._p, cycle[i + 1]._uv, normal));

				halves[1].append(Vertex(cycle[0]._p, cycle[0]._uv, -normal));
				halves[1].append(Vertex(cycle[i + 1]._p, cycle[i + 1]._uv, -normal));
				halves[1].append(Vertex(cycle[i]._p, cycle[i]._uv, -normal));
			}
		}
	}

	/**
	* Triangulation for debugging purposes. Only add a single triangle to test for different vertices.
	* Does not hold up for an actual full triangulation and will cause crashes later on.
	* @param halves - The positive and negatives halves of the mesh to split
	* @param cycle - Cyclic perimeter of the polygon to triangulate
	* @param tri - Index of the vertices to test
	*/
	void MeshSplitter::debugTriangulation(std::vector<MeshData>& halves,
		const CyclicBorder& cycle, int testIndex) const {

		glm::vec3 normal = cycle.getNormal();

		int toTest = testIndex % cycle.cycleSize();

		if (testIndex < 2) testIndex = 2;

		int triCCW = isCCW(cycle[0], cycle[1], cycle[toTest]);

		if (triCCW != 0) {

			halves[0].append(Vertex(cycle[0]._p, cycle[0]._uv, normal));
			halves[0].append(Vertex(cycle[1]._p, cycle[1]._uv, normal));
			halves[0].append(Vertex(cycle[toTest]._p, cycle[toTest]._uv, normal));

			halves[1].append(Vertex(cycle[0]._p, cycle[0]._uv, -normal));
			halves[1].append(Vertex(cycle[toTest]._p, cycle[toTest]._uv, -normal));
			halves[1].append(Vertex(cycle[1]._p, cycle[1]._uv, -normal));
		}
	}

	/**
	* Calculates the area of a triangle defined by its vertices.
	* @param p0 - First vertex
	* @param p1 - Second vertex
	* @param p2 - Third vertex
	* @param coords - Dimensions to use for the x & y 2D coordinates
	* @return Area of the triangle p0p1p2
	*/
	static float triArea(const glm::vec3 p0, const glm::vec3 p1, const glm::vec3 p2,
		glm::vec2 coords) {

		return 0.5f * (std::fabsf(p0[coords[0]] * (p1[coords[1]] - p2[coords[1]]) +
			p1[coords[0]] * (p2[coords[1]] - p0[coords[1]]) +
			p2[coords[0]] * (p0[coords[1]] - p1[coords[1]])));
	}

	/**
	* Test if a given vertex of polygon is an ear of that polygon.
	* An ear is defined as a convex vertex that doesn't have any vertex that lies inside the triangle
	* that it forms with its neighbouring vertices.
	* @param cycle - Cyclic permieter of the polygon
	* @param earId - Index of the vertex to test
	* @return If the vertex is an ear
	*/
	static bool isEar(const CyclicBorder& cycle, int earId) {

		int winding = isCCW(cycle[earId - 1], cycle[earId], cycle[earId + 1]);

		// Only convex vertices can qualify as ears.
		if (winding != cycle.getWinding()) return false;

		float totalArea = triArea(cycle[earId - 1]._p,
			cycle[earId]._p, cycle[earId + 1]._p, cycle.getPlaneCoordinates());

		// Check if another vertex is inside the triangle
		for (int i = 0; i < cycle.cycleSize(); i++) {

			// Ignore vertices that are part of the triangle
			if (i == earId ||
				i == (earId + cycle.cycleSize() - 1) % cycle.cycleSize()
				|| i == (earId + 1) % cycle.cycleSize()) continue;

			float area1 = triArea(cycle[i]._p,
				cycle[earId]._p, cycle[earId + 1]._p, cycle.getPlaneCoordinates());
			float area2 = triArea(cycle[earId - 1]._p,
				cycle[i]._p, cycle[earId + 1]._p, cycle.getPlaneCoordinates());
			float area3 = triArea(cycle[earId - 1]._p,
				cycle[earId]._p, cycle[i]._p, cycle.getPlaneCoordinates());

			// Areas are approximately the same
			if (std::fabsf((area1 + area2 + area3) - totalArea) <= 0.00001f) return false;
		}

		return true;
	}

	/**
	* Polygon triangulation using the ear trimming algorithm.
	* Since by the two ear theorem all polygons have at least two ears and
	* ears contain no other vertices, triangles containing ears get progressively
	* removed from the polygon.
	* This algorithm works on any convex or concave polygons.
	* @param halves - The positive and negatives halves of the mesh to split
	* @param cycle - Cyclic perimeter of the polygon to triangulate
	*/
	void MeshSplitter::earTrimming(std::vector<MeshData>& halves, CyclicBorder& cycle) const {

		glm::vec3 normal = cycle.getNormal();

		bool progressed;

		do {

			progressed = false;

			for (int i = 0; i < cycle.cycleSize(); i++) {

				// Create triangle from the ear. It can then be removed from the polygon
				if (isEar(cycle, i)) {

					halves[0].append(Vertex(cycle[i - 1]._p, cycle[i - 1]._uv, normal));
					halves[0].append(Vertex(cycle[i]._p, cycle[i]._uv, normal));
					halves[0].append(Vertex(cycle[i + 1]._p, cycle[i + 1]._uv, normal));

					halves[1].append(Vertex(cycle[i + 1]._p, cycle[i + 1]._uv, normal));
					halves[1].append(Vertex(cycle[i]._p, cycle[i]._uv, normal));
					halves[1].append(Vertex(cycle[i - 1]._p, cycle[i - 1]._uv, normal));

					cycle.remove(i);

					progressed = true;
					break;
				}
			}
		} while (cycle.cycleSize() > 2 && progressed);
	}
}