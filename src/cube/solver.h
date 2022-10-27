#pragma once

// source: https://github.com/Cubically/thistlethwaite

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <queue>
#include <algorithm>
#include <list>
#include "state.h"
#include "move.h"

namespace rubik {

	std::vector<Move> getSolution(CubeState problem);
	std::queue<Move> optimizeSolution(std::vector<Move> solution);
}

