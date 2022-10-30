#pragma once

// source: https://github.com/Cubically/thistlethwaite

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <queue>
#include <algorithm>
#include "state.h"
#include "move.h"

namespace rubik {

	const unsigned static int THISTLETHWAITE_PHASE_COUNT = 4;
	const unsigned static int KOCIEMBA_PHASE_COUNT = 2;

	std::vector<Move> thistlethwaite(CubeState problem);
	std::vector<Move> kociemba(CubeState problem);
	std::queue<Move> optimizeSolution(std::vector<Move> solution);
}

