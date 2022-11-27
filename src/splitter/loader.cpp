#include "loader.h"

namespace splr {

	/**
	* Load the data inside of an obj file into vectors of vertices.
	* @param path - location of the file
	* @param finalVerts - Storage for the vertex position
	* @param finalUVs - Storage for the vertex 2D coordinates
	* @param finalNormals - Storage for the vertex normals
	* @return if the reading of the file was succesfull
	*/
	bool loadObj(const char* path, MeshData& mesh) {

		std::vector<unsigned int> vertIndices, uvIndices, normalIndices;
		std::vector<glm::vec3> tempVerts;
		std::vector<glm::vec2> tempUV;
		std::vector<glm::vec3> tempNormals;

		FILE* file;
		fopen_s(&file, path, "r");
		if (file == NULL) {
			std::cerr << "ERROR: The given OBJ file cannot be found." << std::endl;
			return false;
		}

		while (1) {

			char lineHeader[128];
			int res = fscanf_s(file, "%s", lineHeader, sizeof(lineHeader));
			if (res == EOF)
				break; // Reached the end of the file

			// Vertex position
			if (strcmp(lineHeader, "v") == 0) {
				glm::vec3 vertex;
				fscanf_s(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
				tempVerts.push_back(vertex);
			}
			// Vertex texture coordinates
			else if (strcmp(lineHeader, "vt") == 0) {
				glm::vec2 uv;
				fscanf_s(file, "%f %f\n", &uv.x, &uv.y);
				tempUV.push_back(uv);
			}
			// Vertex normal coordinates
			else if (strcmp(lineHeader, "vn") == 0) {
				glm::vec3 normal;
				fscanf_s(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
				tempNormals.push_back(normal);
			}
			// indices
			else if (strcmp(lineHeader, "f") == 0) {

				std::string vert1, vert2, vert3;
				unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
				int matches = fscanf_s(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n",
					&vertexIndex[0], &uvIndex[0], &normalIndex[0],
					&vertexIndex[1], &uvIndex[1], &normalIndex[1],
					&vertexIndex[2], &uvIndex[2], &normalIndex[2]);
				if (matches != 9) {
					std::cerr << "ERROR: Check if the UVs and the Normals "
						<< "of the mesh are defined in the OBJ file." << std::endl;
					return false;
				}

				vertIndices.push_back(vertexIndex[0]);
				vertIndices.push_back(vertexIndex[1]);
				vertIndices.push_back(vertexIndex[2]);
				uvIndices.push_back(uvIndex[0]);
				uvIndices.push_back(uvIndex[1]);
				uvIndices.push_back(uvIndex[2]);
				normalIndices.push_back(normalIndex[0]);
				normalIndices.push_back(normalIndex[1]);
				normalIndices.push_back(normalIndex[2]);
			}
		}

		for (int i = 0; i < vertIndices.size(); i++) {

			mesh.append(Vertex(
				tempVerts[vertIndices[i] - 1],
				tempUV[uvIndices[i] - 1],
				tempNormals[normalIndices[i] - 1]
			));
		}
	}

	/**
	* Seperates a mesh into two meshes with a plane.
	* @param planeNorm - surface normal of the plane
	* @param planeDist - distance of the plane from the origin
	*/
	std::vector<MeshData> MeshData::splitMeshAlongPlane(const glm::vec3 planeNorm,
		const float planeDist) const {

		MeshData positiveMesh;
		MeshData negativeMesh;

		std::vector<Vertex> border;

		for (int v = 0; v < size(); v += 3) {
			// for each triangle
			std::vector<uint8_t> positives;
			std::vector<uint8_t> negatives;
			std::vector<uint8_t> zeros;

			for (int i = 0; i < 3; i++) {
				// plug into ax + by + cz - d
				float distance = glm::dot(pos[v + i], planeNorm) - planeDist;
				if (distance > 0) positives.push_back(i);
				else if (distance < 0) negatives.push_back(i);
				else zeros.push_back(i);
			}

			if (negatives.size() == 0) {
				// add to positive mesh (no negative corners)

				for (int i = 0; i < 3; i++) {
					positiveMesh.append((*this)[v + i]);
				}
			}
			else if (positives.size() == 0) {
				// add to negative mesh (no positive corners)
				for (int i = 0; i < 3; i++) {
					negativeMesh.append((*this)[v + i]);
				}
			}
			else if (positives.size() == negatives.size()) {
				// one corner on the plane and the other two on each sides

				splitTriInHalf(positiveMesh, negativeMesh, border,
					planeNorm, planeDist, v, positives, negatives, zeros);

			}
			else {
				// two corners are on one side and the other one is on the other side
				splitTriInThree(positiveMesh, negativeMesh, border,
					planeNorm, planeDist, v, positives, negatives, zeros);
			}
		}
		// ear trimming to triangulate the face
		if (!border.empty()) {

			recreateFace(positiveMesh, negativeMesh, border, planeNorm, planeDist);
		}

		std::vector<MeshData> ret(2);
		ret[0] = positiveMesh;
		ret[1] = negativeMesh;

		return ret;
	}

	/**
	* Split a tri that is perfectly in the middle of a plane
	* @param posMesh
	* @param negMesh
	* @param border
	* @param planeNorm
	* @param planeDist
	* @param v
	* @param positives
	* @param negatives
	* @param zeros
	*/
	void MeshData::splitTriInHalf(MeshData& posMesh, MeshData& negMesh,
		std::vector<Vertex>& border, const glm::vec3 planeNorm, float planeDist,
		int v, const std::vector<uint8_t>& positives, const std::vector<uint8_t>& negatives,
		const std::vector<uint8_t>& zeros) const {

		glm::vec3 dir = pos[positives[0] + v] -
			pos[negatives[0] + v];

		float t = planeDist - glm::dot(planeNorm, pos[negatives[0] + v]);
		t /= glm::dot(planeNorm, dir);

		// interpolate to point
		glm::vec3 p = pos[negatives[0] + v] + t * dir;

		glm::vec2 uv = uvs[negatives[0] + v] +
			(uvs[positives[0] + v] - uvs[negatives[0] + v]) *
			t / glm::length(dir);

		glm::vec3 n = norms[negatives[0] + v] +
			(norms[positives[0] + v] - norms[negatives[0] + v]) *
			t / glm::length(dir);

		Vertex intersection = Vertex(p, uv, n);

		// Winding not ok depending on orientation
		// neutral -> positives -> negatives
		// else neutral -> negatives -> positives
		bool windingOrder = ((zeros[0] + 1) % 3) == positives[0];

		int start = zeros[0];

		if (windingOrder) {
			// 0 1+ p
			posMesh.append((*this)[start + v]);
			posMesh.append((*this)[(start + 1) % 3 + v]);
			posMesh.append(intersection);

			// P 2- 0
			negMesh.append(intersection);
			negMesh.append((*this)[(start + 2) % 3 + v]);
			negMesh.append((*this)[start + v]);
		}
		else {
			// 0 1+ p
			negMesh.append((*this)[start + v]);
			negMesh.append((*this)[(start + 1) % 3 + v]);
			negMesh.append(intersection);

			// P 2- 0
			posMesh.append(intersection);
			posMesh.append((*this)[(start + 2) % 3 + v]);
			posMesh.append((*this)[start + v]);
		}

		/*
		* When the triangle is very slim and there is only one intersection.
		* TODO won't this break the pairs of edges?
		*/
		if (intersection != (*this)[start + v]) {
			border.push_back(intersection);
			border.push_back((*this)[start + v]);
		}
	}

	/**
	* Split a triangle that is intersected with a plane into three smaller triangles
	* @param posMesh
	* @param negMesh
	* @param border
	* @param planeNorm
	* @param planeDist
	* @param v
	* @param positives
	* @param negatives
	* @param zeros
	*/
	void MeshData::splitTriInThree(MeshData& posMesh, MeshData& negMesh,
		std::vector<Vertex>& border, const glm::vec3 planeNorm, float planeDist,
		int v, const std::vector<uint8_t>& positives, const std::vector<uint8_t>& negatives,
		const std::vector<uint8_t>& zeros) const {

		bool positiveBias = negatives.size() == 1;
		int startId = positiveBias ? negatives[0] : positives[0];

		// p1i = +p1 to -p0 and plane
		glm::vec3 dir = pos[(startId + 1) % 3 + v] - pos[startId + v];

		float t = planeDist - glm::dot(planeNorm, pos[startId + v]);
		t /= glm::dot(planeNorm, dir);

		// interpolate to point
		glm::vec3 p = pos[startId + v] + t * dir;

		glm::vec2 uv = uvs[startId + v] +
			(uvs[(startId + 1) % 3 + v] - uvs[startId + v]) *
			t / glm::length(dir);

		glm::vec3 n = norms[startId + v] +
			(norms[(startId + 1) % 3 + v] - norms[startId + v]) *
			t / glm::length(dir);

		Vertex inter1 = Vertex(p, uv, n);

		// p2i = +p2 to -p0 and plane
		dir = pos[(startId + 2) % 3 + v] - pos[startId + v];

		t = planeDist - glm::dot(planeNorm, pos[startId + v]);
		t /= glm::dot(planeNorm, dir);

		// interpolate to point
		p = pos[startId + v] + t * dir;

		uv = uvs[startId + v] +
			(uvs[(startId + 2) % 3 + v] - uvs[startId + v]) *
			t / glm::length(dir);

		n = norms[startId + v] +
			(norms[(startId + 2) % 3 + v] - norms[startId + v]) *
			t / glm::length(dir);

		Vertex inter2 = Vertex(p, uv, n);

		if (positiveBias) {
			// t1 = (p1i, p2i, -p0)
			negMesh.append(inter1);
			negMesh.append(inter2);
			negMesh.append((*this)[startId + v]);
			// t2 = (+p1, +p2, p1i)
			posMesh.append(inter1);
			posMesh.append((*this)[(startId + 1) % 3 + v]);
			posMesh.append((*this)[(startId + 2) % 3 + v]);
			// t3 = (+p2, p2i, p1i)
			posMesh.append((*this)[(startId + 2) % 3 + v]);
			posMesh.append(inter2);
			posMesh.append(inter1);
		}
		else {
			// possibly change winding order
			// t1 = (p1i, p2i, -p0)
			posMesh.append(inter1);
			posMesh.append(inter2);
			posMesh.append((*this)[startId + v]);
			// t2 = (+p1, +p2, p1i)
			negMesh.append((*this)[(startId + 1) % 3 + v]);
			negMesh.append((*this)[(startId + 2) % 3 + v]);
			negMesh.append(inter1);
			// t3 = (+p2, p2i, p1i)
			negMesh.append((*this)[(startId + 2) % 3 + v]);
			negMesh.append(inter2);
			negMesh.append(inter1);
		}

		/*
		* When the triangle is very slim and there is only one intersection.
		* TODO won't this break the pairs of edges?
		*/
		if (inter1 != inter2) {
			border.push_back(inter2);
			border.push_back(inter1);
		}
	}

	/**
	* Determines whether the triangle formed by three vertices is wound
	* in a counter-clockwise order when viewed from a point.
	* @param p - viewpoint
	* @param v0 - Presumably most clockwise vertex
	* @param v1 - Middle vertex
	* @param v2 - Presumably most counter-clockwise vertex
	* @return whether the triangle is ccw
	*/
	int isCCW(const glm::vec3& p, const Vertex& v0, const Vertex& v1, const Vertex& v2, bool exact) {

		glm::vec3 triN = glm::cross(v1.p - v0.p,
			v2.p - v0.p);

		if (approximates(glm::vec3(0.f), triN) && !exact) return 0;

		triN = glm::normalize(triN);

		int otherTest = glm::dot(p - v0.p, triN) >= 0 ? 1 : 2; // point faces camera

		return otherTest;
	}

	/**
	* Calculate the shape to close the mesh cut by the plane.
	* @param posMesh - mesh on the positive side of the slicing plane
	* @param negMesh - mesh on the negative side of the slicing plane
	* @param border - list of pairs of neighbouring vertices in the border
	* @param planeNorm - normal of the slicing plane
	* @param planeDist - distance of the plane to the origin
	*/
	void MeshData::recreateFace(MeshData& posMesh, MeshData& negMesh,
		std::vector<Vertex>& border, const glm::vec3 planeNorm, float planeDist) const {

		// find the winding of the first tri
		int desiredWinding = (glm::dot(glm::vec3(0, 0, 30) - planeNorm, -planeNorm) >= 0) ? 1 : 2;

		// TODO fix winding bugs
		std::vector<Vertex> cyclic = buildCyclicBorder(posMesh, negMesh, border, planeNorm, desiredWinding);

		// TODO (make sure it works with proper winding)
		//earTrimming(posMesh, negMesh, cyclic, planeNorm, desiredWinding);

		///*
		for (int i = 1; i < cyclic.size() - 1; i++) {

			posMesh.append(Vertex(cyclic[0].p, cyclic[0].uv, planeNorm));
			posMesh.append(Vertex(cyclic[i].p, cyclic[i].uv, planeNorm));
			posMesh.append(Vertex(cyclic[i + 1].p, cyclic[i + 1].uv, planeNorm));

			negMesh.append(Vertex(cyclic[0].p, cyclic[0].uv, -planeNorm));
			negMesh.append(Vertex(cyclic[i + 1].p, cyclic[i + 1].uv, -planeNorm));
			negMesh.append(Vertex(cyclic[i].p, cyclic[i].uv, -planeNorm));
		}
		//*/
	}

	/**
	* Create a cyclic list of the perimeter of the border.
	*
	* TODO: This doesn't work correctly if the first vertex is concave.
	* trying to find a convex vert to calculate the winding
	*
	* @param posMesh - mesh on the positive side of the slicing plane
	* @param negMesh - mesh on the negative side of the slicing plane
	* @param border - list of pairs of neighbouring vertices in the border
	* @param planeNorm - normal of the slicing plane
	* @param desiredWinding - orientation of the list. true for ccw and false for cw
	* @return list of vertices ordered in the desired winding
	*/
	std::vector<Vertex> MeshData::buildCyclicBorder(MeshData& posMesh, MeshData& negMesh, std::vector<Vertex>& border,
		const glm::vec3 planeNorm, int desiredWinding) const {

		// find the winding of the first tri
		int planeAxis = (planeNorm.y != 0) * 1 + (planeNorm.z != 0) * 2;
		std::vector<Vertex> cyclic;

		// To test with other than shifted cube, cyclic is initialized as empty
		findWinding(posMesh, negMesh, border, cyclic, planeNorm, planeAxis, desiredWinding);

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
				// TODO: experimental, doesnt work for monkey and egg
				/*
				else if (border[i] == cyclic.front()) {
					foundEdge = true;

					std::cout << "front : " << i << std::endl;

					cyclic.emplace(cyclic.begin(), border[i - 2 * (i % 2) + 1]);

					border.erase(border.begin() + (i - (i % 2)),
						border.begin() + (i - (i % 2)) + 2);
				}
				//*/
				i++;
			}

			fullCircle = border.size() == 0 || !foundEdge;

			// add it manualy
			// TODO: potential problem with concave shapes (doesn't ear trim)
			if (!foundEdge) {

				std::cout << "yo" << std::endl;

				for (int j = 0; j < border.size() - 1; j++) {

					int triCCW = isCCW(glm::vec3(0, 0, 30), cyclic[0],
						border[j], border[j + 1]); // point faces camera

					if (triCCW == desiredWinding) {
						posMesh.append(Vertex(cyclic[0].p, cyclic[0].uv, planeNorm));
						posMesh.append(Vertex(border[j].p, border[j].uv, planeNorm));
						posMesh.append(Vertex(border[j + 1].p, border[j + 1].uv, planeNorm));

						negMesh.append(Vertex(cyclic[0].p, cyclic[0].uv, -planeNorm));
						negMesh.append(Vertex(border[j + 1].p, border[j + 1].uv, -planeNorm));
						negMesh.append(Vertex(border[j].p, border[j].uv, -planeNorm));

					}
					else {
						posMesh.append(Vertex(cyclic[0].p, cyclic[0].uv, planeNorm));
						posMesh.append(Vertex(border[j + 1].p, border[j + 1].uv, planeNorm));
						posMesh.append(Vertex(border[j].p, border[j].uv, planeNorm));

						negMesh.append(Vertex(border[j].p, border[j].uv, -planeNorm));
						negMesh.append(Vertex(border[j + 1].p, border[j + 1].uv, -planeNorm));
						negMesh.append(Vertex(cyclic[0].p, cyclic[0].uv, -planeNorm));
					}
				}
			}
		}

		return cyclic;
	}


	void MeshData::findWinding(MeshData& posMesh, MeshData& negMesh, std::vector<Vertex>& border,
		std::vector<Vertex>& cyclic, const glm::vec3 planeNorm, int planeAxis, int desiredWinding) const {

		int x = (planeAxis + 1) % 3;
		int y = (planeAxis + 2) % 3;

		int convex = 0;

		float maxX = border[0].p[x];
		float maxY = border[0].p[y];

		for (int i = 1; i < border.size(); i++) {

			if (border[i].p[x] > maxX) {
				convex = i;
				maxX = border[i].p[x];
				maxY = border[i].p[y];
			}
			else if (border[i].p[x] == maxX) {
				if (border[i].p[y] > maxY) {
					convex = i;
					maxX = border[i].p[x];
					maxY = border[i].p[y];
				}
			}
		}

		int neighbour = convex - 2 * (convex % 2) + 1;

		// TODO I need to start at 0 for certain meshes, but why??
		for (int i = 0; i < border.size() && cyclic.size() == 0; i++) {

			bool isZero = (border[i] == border[convex]) && i != convex;

			if (isZero) {

				Vertex vMiddle = border[convex];
				Vertex vExtrem = border[neighbour];

				int triCCW = isCCW(glm::vec3(0, 0, 30), vExtrem,
					vMiddle, border[i - 2 * (i % 2) + 1], true);

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

				// always delete what comes later in the vector first
				if (i > convex) {
					border.erase(border.begin() + (i - (i % 2)),
						border.begin() + (i - (i % 2)) + 2);
					border.erase(border.begin() + std::min(convex, neighbour),
						border.begin() + std::min(convex, neighbour) + 2);
				}
				else {
					border.erase(border.begin() + std::min(convex, neighbour),
						border.begin() + std::min(convex, neighbour) + 2);
					border.erase(border.begin() + (i - (i % 2)),
						border.begin() + (i - (i % 2)) + 2);
				}
			}
		}
	}

	/**
	* Calculate the area of the triangle with the given vertices
	* @param x - index of the x dimension to use
	* @param y - index of the y dimension to use
	*/
	double triArea(const glm::vec3 p0, const glm::vec3 p1, const glm::vec3 p2, int x, int y) {

		return (std::fabs(p0[x] * (p2[y] - p1[y]) +
			p1[x] * (p2[y] - p0[y]) +
			p2[x] * (p1[y] - p0[y])));
	}

	/// EXPERIMENTAL FOR NOW /// 

	/**
	* Determine whether the vertex constitutes an ear.
	* An ear is a convex vertex where the triangle with its neighbouring
	* vertices contains no other points.
	* @param cyclic - Ordered list of the vertices
	* @param earId - index of the ear to check
	* @param desiredWinding - orientation of a tri with a concave vertex
	*/
	bool isEar(const std::vector<Vertex>& cyclic, int earId, int plane, int desiredWinding) {

		int vPrev = (earId + cyclic.size() - 1) % cyclic.size();
		int vNext = (earId + 1) % cyclic.size();

		int x = (plane + 1) % 3;
		int y = (x + 1) % 3;

		// TODO With very thin tris, the area is almost 0 and the cross product is very small
		// this makes determining if it is an ear very difficult
		// also there sometimes seems to be duplicate vertices which breaks this

		double insideArea = triArea(cyclic[vPrev].p, cyclic[earId].p, cyclic[vNext].p, x, y);

		if (insideArea == 0) {
			return true;
		}

		int isConvex = isCCW(glm::vec3(0, 0, 30), cyclic[vPrev], cyclic[earId], cyclic[vNext]);

		if (isConvex == 0)
			return true;

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
			double area1 = triArea(cyclic[index].p, cyclic[earId].p, cyclic[vNext].p, x, y),
				area2 = triArea(cyclic[vPrev].p, cyclic[index].p, cyclic[vNext].p, x, y),
				area3 = triArea(cyclic[vPrev].p, cyclic[earId].p, cyclic[index].p, x, y);

			// If the sum is equal to the total, the point is inside the triangle
			// thus, we don't have an ear
			if (area1 + area2 + area3 == insideArea) return false;
		}

		// TODO //

		return true;
	}

	/**
	* TODO, doesnt work with most meshes.
	* They end up with more than 4 tris that are all concave for some reason.
	* Maybe the check for convexity is wrong.
	*
	* Possible solution => keep a list of convex vertices and update it every time a tri is removed
	* for all the affected verts. Then pick from these vertex to trimm.
	*/
	void MeshData::earTrimming(MeshData& posMesh, MeshData& negMesh, std::vector<Vertex>& cyclic,
		const glm::vec3 planeNorm, int desiredWinding) const {

		// TODO //
		// Is this corner an ear?
		//	- Convex corner (orientation of the tri is not the desired orientation)
		//	- No vertices are inside the triangle (look at area of small tris)
		// Form a tri with the ear and remove the ear.
		//

		// Make it does what it needs to do.
		int planeAxis = (planeNorm.y != 0) * 1 + (planeNorm.z != 0) * 2;

		while (cyclic.size() >= 3) {

			bool foundEar = false;
			int i = 0;

			while (!foundEar && i < cyclic.size()) {
				// doesnt work correctly
				if (isEar(cyclic, i, planeAxis, desiredWinding)) {

					foundEar = true;

					Vertex vFront(cyclic[(i + cyclic.size() - 1) % cyclic.size()].p,
						cyclic[(i + cyclic.size() - 1) % cyclic.size()].uv,
						planeNorm);

					Vertex vBack(cyclic[(i + 1) % cyclic.size()].p,
						cyclic[(i + 1) % cyclic.size()].uv,
						planeNorm);

					// add to meshes (TODO fix normals)
					posMesh.append(vFront);
					posMesh.append(Vertex(cyclic[i].p, cyclic[i].uv, planeNorm));
					posMesh.append(vBack);

					negMesh.append(vBack);
					negMesh.append(Vertex(cyclic[i].p, cyclic[i].uv, planeNorm));
					negMesh.append(vFront);

					// remove from list

					cyclic.erase(cyclic.begin() + i, cyclic.begin() + i + 1);
				}

				i++;
			}

			// add last tri (NORMALY THIS SHOULD NOT BE CONSIDERED)
			if (cyclic.size() == 3) {
				int triCCW = isCCW(glm::vec3(0, 0, 30), cyclic[0], // doesnt return the right thing?
					cyclic[1], cyclic[2]); // point faces camera

				if (triCCW == desiredWinding) {
					posMesh.append(Vertex(cyclic[0].p, cyclic[0].uv, planeNorm));
					posMesh.append(Vertex(cyclic[1].p, cyclic[1].uv, planeNorm));
					posMesh.append(Vertex(cyclic[2].p, cyclic[2].uv, planeNorm));

					negMesh.append(Vertex(cyclic[0].p, cyclic[0].uv, -planeNorm));
					negMesh.append(Vertex(cyclic[2].p, cyclic[2].uv, -planeNorm));
					negMesh.append(Vertex(cyclic[1].p, cyclic[1].uv, -planeNorm));

				}
				else {
					posMesh.append(Vertex(cyclic[0].p, cyclic[0].uv, planeNorm));
					posMesh.append(Vertex(cyclic[2].p, cyclic[2].uv, planeNorm));
					posMesh.append(Vertex(cyclic[1].p, cyclic[1].uv, planeNorm));

					negMesh.append(Vertex(cyclic[1].p, cyclic[1].uv, -planeNorm));
					negMesh.append(Vertex(cyclic[2].p, cyclic[2].uv, -planeNorm));
					negMesh.append(Vertex(cyclic[0].p, cyclic[0].uv, -planeNorm));
				}

				cyclic.clear();
			}
		}
	}
}