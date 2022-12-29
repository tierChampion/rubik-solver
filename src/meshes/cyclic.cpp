#include "cyclic.h"

namespace splr {

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
	int isCCW(const Vertex& v0, const Vertex& v1, const Vertex& v2) {

		glm::vec3 triN = glm::cross(v1._p - v0._p,
			v2._p - v0._p);

		if (triN == glm::vec3(0.f)) return 0;

		triN = glm::normalize(triN);

		// Check if the normal faces the camera. If so, it is CCW.
		int winding = glm::dot(INFINITE_CAMERA_POS - v0._p, triN) >= 0 ? 1 : 2;

		return winding;
	}

	/**
	* Cyclical list of the border.
	* Stores the vertices of a polygon in counter-clockwise or clockwise
	* order depending on what is needed.
	* @param planeNorm - normal of the cutting plane
	*/
	CyclicBorder::CyclicBorder(const glm::vec3 planeNorm) {

		this->_cycleNormal = planeNorm;

		// Coordinate to be removed from vertex to get to Plane Space
		this->_axis = (planeNorm.y != 0) * 1 + (planeNorm.z != 0) * 2;
		this->_flatCoords = glm::ivec2((_axis + 1) % 3, (_axis + 2) % 3);

		// Winding that a convex vertex should have
		this->_desiredWinding = (glm::dot(INFINITE_CAMERA_POS - planeNorm,
			-planeNorm) >= 0) ? 1 : 2;
	}

	/**
	* Add a known unordered edge to the cycle
	* @param v1 - First vertex of the edge
	* @param v2 - Second vertex of the edge
	*/
	void CyclicBorder::appendEdge(Vertex v1, Vertex v2) {

		_verts.push_back(v1);
		_verts.push_back(v2);
	}

	/**
	* Organize the edges for each vertices
	*/
	void CyclicBorder::createEdges() {

		int id = 0;

		for (int i = 0; i < _verts.size(); i += 2) {

			for (int j = 0; j < 2; j++) {

				auto duplicate = std::find(_verts.begin(), _verts.end(), _verts[i + j]);
				int pos = duplicate - _verts.begin();

				// If this vertex was already seen, make the ids equal.
				if (pos < i + j) {

					_verts[i + j].setId(duplicate->_id);
				}
				// If it is new, give it a new id.
				else {
					_verts[i + j].setId(id);
					id++;
				}
			}

			// Add the edge in both directions.
			_edges[_verts[i]].push_back(_verts[i + 1]);
			_edges[_verts[i + 1]].push_back(_verts[i]);
		}
	}

	/**
	* Construct the cyclic border from the known edges.
	*/
	void CyclicBorder::buildBorder() {

		// Find the edges
		createEdges();
		cleanEdges();

		// Find two properly ordered edges
		findWinding();

		bool foundEdge;
		do {
			foundEdge = false;

			// Append a vertex at the front of the cycle
			Vertex f = _cycle.front();
			std::vector<Vertex> fNeighbours = _edges[f];

			if (fNeighbours.size() == 2) {
				if (fNeighbours[0] == _cycle[1]) {
					_cycle.emplace(_cycle.begin(), fNeighbours[1]);
				}
				else {
					_cycle.emplace(_cycle.begin(), fNeighbours[0]);
				}
				foundEdge = true;

				_edges.erase(f);
			}

			if (_cycle.front() == _cycle.back()) break;

			// Append a vertex at the back of the cycle
			Vertex b = _cycle.back();
			std::vector<Vertex> bNeighbours = _edges[b];

			if (bNeighbours.size() == 2) {
				if (bNeighbours[0] == _cycle[_cycle.size() - 2]) {
					_cycle.push_back(bNeighbours[1]);
				}
				else {
					_cycle.push_back(bNeighbours[0]);
				}
				foundEdge = true;

				_edges.erase(b);
			}

			if (_cycle.front() == _cycle.back()) break;

		} while (foundEdge);

		// Make sure the cycle is continuous
		if (_cycle.front() == _cycle.back()) {
			_cycle.pop_back();
		}
	}

	/**
	* Remove redundant edges from the cycle.
	*/
	void CyclicBorder::cleanEdges() {

		bool progressed;

		// Keep on going until all useless vertices have been removed
		do {

			progressed = false;

			int index = 0;
			auto iter = _edges.begin();

			while (iter != _edges.end()) {

				bool currentlyProgressed = false;

				Vertex current = iter->first;
				std::vector<Vertex> neighbours = iter->second;

				if (neighbours.size() == 2) {

					int flat = isCCW(neighbours[0], current, neighbours[1]);

					// Vertex is on a straight edge from neighbour#1 to neighbour#2.
					// We can thus remove it and link neighbour#1 and neighbour#2.
					if (flat == 0) {

						// Link the two neighbours together and remove itself from edges
						std::replace(_edges[neighbours[0]].begin(), _edges[neighbours[0]].end(),
							current, neighbours[1]);
						std::replace(_edges[neighbours[1]].begin(), _edges[neighbours[1]].end(),
							current, neighbours[0]);
						_edges.erase(current);

						// Reset iterator to modified map
						iter = std::next(_edges.begin(), index);

						progressed = true;
						currentlyProgressed = true;
					}
				}

				// Only increment position in the map if no edges were removed
				if (!currentlyProgressed) {
					++index;
					++iter;
				}
			}
		} while (progressed);
	}
	/**
	* Determine the orientation of the first vertex.
	*/
	void CyclicBorder::findWinding() {

		int convex = 0;

		bool foundConvex = false;

		do {

			glm::vec2 dir = glm::circularRand(1.0f);

			int convex = 0;

			float maxA = _verts[0]._p[_flatCoords[0]] * dir[0] + _verts[0]._p[_flatCoords[1]] * dir[1];
			float maxB = _verts[0]._p[_flatCoords[0]] * -dir[1] + _verts[0]._p[_flatCoords[1]] * dir[0];

			// Find the vertex which maximises the two dimensions. 
			// It is necessarly a corner and convex.
			for (int i = 1; i < _verts.size(); i++) {

				if (_verts[i] == _verts[convex]) continue;

				float distA = _verts[i]._p[_flatCoords[0]] * dir[0] + _verts[i]._p[_flatCoords[1]] * dir[1];
				float distB = _verts[i]._p[_flatCoords[0]] * -dir[1] + _verts[i]._p[_flatCoords[1]] * dir[0];

				if (distA > maxA || (distA == maxA && distB > maxB)) {
					convex = i;
					maxA = distA;
					maxB = distB;
				}
			}

			std::vector<Vertex> neighbours = _edges[_verts[convex]];

			if (neighbours.size() != 2) continue;

			int triCCW = isCCW(neighbours[0],
				_verts[convex], neighbours[1]);

			// Flat triangles don't have any logical winding, so ignore them.
			if (triCCW == 0) {
				continue;
			}

			if (triCCW == _desiredWinding) {

				_cycle.push_back(neighbours[0]);
				_cycle.push_back(_verts[convex]);
				_cycle.push_back(neighbours[1]);
			}
			else {
				_cycle.push_back(neighbours[1]);
				_cycle.push_back(_verts[convex]);
				_cycle.push_back(neighbours[0]);
			}

			_edges.erase(_verts[convex]);

			foundConvex = true;

		} while (!foundConvex);
	}
}