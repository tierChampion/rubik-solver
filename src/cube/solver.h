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

	const unsigned static int THISTLETHWAITE_KOCIEMBA_PHASE_COUNT = 3;

	std::vector<Move> thistlethwaite_kociemba(CubeState problem);
	std::queue<Move> optimizeSolution(std::vector<Move> solution);
}

