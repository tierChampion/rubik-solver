#pragma once

#include "model.h"
#include "state.h"
#include "move.h"
#include "solver.h"

namespace rubik {
	class Cube {

		CubeModel _model;
		CubeState _state;
		bool _solving = false;

	public:
		Cube(bool mirror);
		void update();
		void render(Vao vao, int programId);
		void turnFace(Move move);
		void turnCube(glm::vec2 delta);
		void solve();
		bool isSolving();
		void mix();

		/**
		* Shows the move in the standard rubik's cube format.
		* @param s - stream to later print
		* @param move - move to convert to standard format
		*/
		friend std::ostream& operator<<(std::ostream& s, const Cube& cube) {

			for (int i = 0; i < 40; i++) {
				s << cube._state[i];
			}


			return s << "\n";
		}

	};
}
