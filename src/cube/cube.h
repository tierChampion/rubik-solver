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
		* Show the cube state for debugging purposes
		* @param cube - cube to show the state of
		*/
		friend std::ostream& operator<<(std::ostream& s, const Cube& cube) {

			return s << cube._state;
		}

	};
}
