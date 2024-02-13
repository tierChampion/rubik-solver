#pragma once

#include <vector>
#include <glm/vec2.hpp>

#include "model.h"
#include "state.h"
#include "move.h"
#include "solver.h"

namespace rubik
{
	class Cube
	{
		CubeModel _model;
		CubeState _state;
		CubeType _type;
		bool _solving = false;
		bool _centerOrientation;

	public:
		Cube(CubeType type);
		void update();
		void render(const std::vector<Vao> &vaos, int programId);
		void turnFace(Move move);
		void turnCube(glm::vec2 delta);
		void solve();
		bool isSolving();
		void mix();
		void changeType(CubeType newType);

		friend std::ostream &operator<<(std::ostream &s, const Cube &cube);
	};
}
