#pragma once

// source: https://github.com/Cubically/thistlethwaite

#include <vector>
#include <queue>

#include "cube.h"
#include "move.h"

namespace rubik
{
	const unsigned static int THISTLETHWAITE_KOCIEMBA_PHASE_COUNT = 3;

	std::queue<Move> thistlethwaiteKociemba(Cube* cube, CubeState problem);
	std::vector<Move> solveCenters(CubeState problem);
	std::queue<Move> optimizeSolution(std::vector<Move> solution);
}
