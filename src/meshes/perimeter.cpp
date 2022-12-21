#include "meshsplitter.h"

namespace splr {

	CyclicBorder::CyclicBorder(const glm::vec3 planeNorm) {

		this->_cycleNormal = planeNorm;
		this->_axis = (planeNorm.y != 0) * 1 + (planeNorm.z != 0) * 2;
		// Winding that a convex vertex should have
		this->_desiredWinding = (glm::dot(INFINITE_CAMERA_POS - planeNorm,
			-planeNorm) >= 0) ? 1 : 2;
	}

	void CyclicBorder::appendEdge(const Vertex& v1, const Vertex& v2) {

		_rawVerts.push_back(v1);
		_rawVerts.push_back(v2);

		_rawEdges[v1].push_back(v2);
		_rawEdges[v2].push_back(v1);
	}

	void CyclicBorder::buildBorder(std::vector<MeshData>& halves) {

		findWinding();

		bool fullCycle = false;

		/*
		* TODO:
		* Make a function to clean the edges.
		* Remove excess edges and add ones to single verts.
		*
		* Also possibly create chains of cycles and the merge the extremities
		* to get the full cycles.
		*
		* Remove the verts with 4 edges. It seems like their might be useless verts sometimes
		* like a thin tri that connects to one. This means the vert has 2 valid edges and 2
		* other edges that form a thin tri (they are the same edge)
		*
		* Create the cycle and then order it properly.
		*/

		bool fullCircle = false;

		while (!fullCircle) {

			// for the current extremities of the cyclic list, check if a vertex is equal to it.
			// than you found another edge that can you then know the orientation of.
			int i = 0;
			bool foundEdge = false;

			while (i < _rawVerts.size() && !foundEdge) {

				if (_rawVerts[i] == _cycle.back()) {
					foundEdge = true;

					_cycle.push_back(_rawVerts[i - 2 * (i % 2) + 1]);

					_rawVerts.erase(_rawVerts.begin() + (i - (i % 2)),
						_rawVerts.begin() + (i - (i % 2)) + 2);
				}
				i++;
			}

			fullCircle = _rawVerts.size() == 0 || !foundEdge;

			// Sometimes, shape borders have gaps. Manually add all the tris
			///*
			if (!foundEdge) {

				for (int j = 0; j < _rawVerts.size() - 1; j++) {

					int triCCW = isCCW(_cycle[0],
						_rawVerts[j], _rawVerts[j + 1]);

					if (triCCW == _desiredWinding) {
						halves[0].append(Vertex(_cycle[0]._p, _cycle[0]._uv, _cycleNormal));
						halves[0].append(Vertex(_rawVerts[j]._p, _rawVerts[j]._uv, _cycleNormal));
						halves[0].append(Vertex(_rawVerts[j + 1]._p, _rawVerts[j + 1]._uv, _cycleNormal));

						halves[1].append(Vertex(_cycle[0]._p, _cycle[0]._uv, -_cycleNormal));
						halves[1].append(Vertex(_rawVerts[j + 1]._p, _rawVerts[j + 1]._uv, -_cycleNormal));
						halves[1].append(Vertex(_rawVerts[j]._p, _rawVerts[j]._uv, -_cycleNormal));

					}
					else if (triCCW != 0) {
						halves[0].append(Vertex(_cycle[0]._p, _cycle[0]._uv, _cycleNormal));
						halves[0].append(Vertex(_rawVerts[j + 1]._p, _rawVerts[j + 1]._uv, _cycleNormal));
						halves[0].append(Vertex(_rawVerts[j]._p, _rawVerts[j]._uv, _cycleNormal));

						halves[1].append(Vertex(_rawVerts[j]._p, _rawVerts[j]._uv, -_cycleNormal));
						halves[1].append(Vertex(_rawVerts[j + 1]._p, _rawVerts[j + 1]._uv, -_cycleNormal));
						halves[1].append(Vertex(_cycle[0]._p, _cycle[0]._uv, -_cycleNormal));
					}
				}
			}
			//*/
		}

		if (_cycle.front() == _cycle.back()) {
			_cycle.pop_back();
		}
	}

	void CyclicBorder::findWinding() {

		int x = (_axis + 1) % 3;
		int y = (_axis + 2) % 3;

		int convex = 0;

		float max = 0;

		bool foundConvex = false;

		do {

			float xStr = glm::gaussRand(0.0f, 1.0f);
			float yStr = glm::gaussRand(0.0f, 1.0f);

			int convex = 0;

			float max = _rawVerts[0]._p[x] * xStr + _rawVerts[0]._p[y] * yStr;

			// Find the vertex which maximises the two dimensions, it is necessarly convex
			for (int i = 1; i < _rawVerts.size(); i++) {

				if (_rawVerts[i] == _rawVerts[convex]) continue;

				float dist = _rawVerts[i]._p[x] * xStr + _rawVerts[i]._p[y] * yStr;

				if (dist > max) {
					convex = i;
					max = dist;
				}
			}

			int neighbour = convex - 2 * (convex % 2) + 1;

			Vertex vMiddle = _rawVerts[convex];
			Vertex vExtrem = _rawVerts[neighbour];

			for (int i = convex; i < _rawVerts.size() && _cycle.size() == 0; i++) {

				bool isZero = (_rawVerts[i] == _rawVerts[convex]) && i != convex;

				if (isZero) {

					int triCCW = isCCW(vExtrem,
						vMiddle, _rawVerts[i - 2 * (i % 2) + 1], true);

					if (triCCW == 0) {
						break;
					}

					if (triCCW == _desiredWinding) {

						_cycle.push_back(vExtrem);
						_cycle.push_back(vMiddle);
						_cycle.push_back(_rawVerts[i - 2 * (i % 2) + 1]);
					}
					else {
						_cycle.push_back(_rawVerts[i - 2 * (i % 2) + 1]);
						_cycle.push_back(vMiddle);
						_cycle.push_back(vExtrem);
					}

					// Remove the tris that were processed

					_rawVerts.erase(_rawVerts.begin() + i - (i % 2),
						_rawVerts.begin() + i - (i % 2) + 2);
					_rawVerts.erase(_rawVerts.begin() + std::min(convex, neighbour),
						_rawVerts.begin() + std::min(convex, neighbour) + 2);

					foundConvex = true;

				}
			}
		} while (!foundConvex);
	}

}