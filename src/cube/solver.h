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
using namespace std;


/**********************************************************************
 *
 * A cube 'state' is a vector<int> with 40 entries, the first 20
 * are a permutation of {0,...,19} and describe which cubie is at
 * a certain position (regarding the input ordering). The first
 * twelve are for edges, the last eight for corners.
 *
 * The last 20 entries are for the orientations, each describing
 * how often the cubie at a certain position has been turned
 * counterclockwise away from the correct orientation. Again the
 * first twelve are edges, the last eight are corners. The values
 * are 0 or 1 for edges and 0, 1 or 2 for corners.
 *
 **********************************************************************/

namespace rubik {

	inline int inverse(int move) {
		return move + 2 - 2 * (move % NUM_MOVES_PER_FACE);
	}

	vector<Move> getSolution(CubeState problem);
	queue<Move> optimizeSolution(vector<Move> solution);
}

