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

		glm::vec3 normal = cycle.getNormal();

		// make it max once
		int counter = 0;

		while (cycle.vertexCount() > 2 && counter < 1) {

			// Build the cycle of the perimeter
			cycle.buildBorder();

			// Naive triangulation (only give a correct result on convex polygons)
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

			cycle.clearCycle();
			counter++;
		}
	}
}