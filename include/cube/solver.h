#pragma once

// source: https://github.com/Cubically/thistlethwaite

#include <vector>
#include <queue>

#include "state.h"
#include "move.h"

namespace rubik
{
	const unsigned static int THISTLETHWAITE_KOCIEMBA_PHASE_COUNT = 3;

	std::vector<Move> thistlethwaiteKociemba(CubeState problem);
	std::vector<Move> solveCenters(CubeState problem);
	std::queue<Move> optimizeSolution(std::vector<Move> solution);
}
