#pragma once

#include "model.h"
#include "state.h"
#include "move.h"
#include "solver.h"

namespace rubik {
	class Cube {

		CubeModel model;
		CubeState state;

	public:
		Cube(bool mirror);
		void update();
		void render(Vao vao, int programId);
		void turnFace(Move move);
		void turnCube(glm::vec2 delta);
		void solve();
		void mix();

	};
}
