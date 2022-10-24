#include "state.h"

namespace rubik {
	CubeState::CubeState() {

		_state = std::vector<int>(40, 0);

		for (int i = 0; i < 20; i++) {
			_state[i] = i;
		}
	}

	CubeState::CubeState(std::vector<int>& s) {

		_state = s;
	}

	CubeState CubeState::applyMove(int move) const {
		int turns = move % NUM_MOVES_PER_FACE + 1;
		int face = move / NUM_MOVES_PER_FACE;

		std::vector<int> current_state = this->_state;

		while (turns--) {
			std::vector<int> oldState = current_state;

			for (int i = 0; i < NUM_CORNERS; i++) {
				int isCorner = i > 3;
				int target = AFFECTED_CUBIES[face][i] + isCorner * NUM_EDGES;
				int killer = AFFECTED_CUBIES[face][(i & 3) == 3 ? i - 3 : i + 1] + isCorner * NUM_EDGES;
				int orientationDelta = (i < 4) ? (face > 1 && face < 4) : (face < 2) ? 0 : 2 - (i & 1);

				/*
				Move and change orientation
				*/
				current_state[target] = oldState[killer];
				current_state[target + TOTAL_NUM_CUBIES] = oldState[killer + TOTAL_NUM_CUBIES] + orientationDelta;

				if (!turns)
					current_state[target + TOTAL_NUM_CUBIES] %= 2 + isCorner;
			}
		}

		return current_state;
	}

	std::vector<int> CubeState::id(unsigned int phase) const {
		// --> Phase 1: Edge orientations (orientation of the 12 first cubies)
		if (phase < 2)
			// orientation of the edges
			return std::vector<int>(_state.begin() + TOTAL_NUM_CUBIES, _state.begin() + TOTAL_NUM_CUBIES + NUM_EDGES);

		// --> Phase 2: Corner orientations, E slice edges 
		// (12 edges as int in index 0 and orientation of corners in rest)
		if (phase < 3) {
			// orientation of the corners
			std::vector<int> result(_state.begin() + TOTAL_NUM_CUBIES + NUM_EDGES - 1, _state.begin() + 40);
			// store each state[e] in bits of result[0]. Store 0 if state[0] < 8 else store 1
			for (int e = 0; e < NUM_EDGES; e++)
				result[0] |= (_state[e] / 8) << e;

			return result;
		}

		// --> Phase 3: Edge slices M and S, corner tetrads, overall parity
		if (phase < 4) {
			std::vector<int> result(3);
			// TODO check something for edges
			for (int e = 0; e < NUM_EDGES; e++)
				result[0] |= ((_state[e] > 7) ? 2 : (_state[e] & 1)) << (2 * e);
			// TODO check something for corners
			for (int c = 0; c < NUM_CORNERS; c++)
				result[1] |= ((_state[c + NUM_EDGES] - NUM_EDGES) & 5) << (3 * c);
			// Check if all corners are properly ordered (^ is equivalent to &&)
			for (int i = NUM_EDGES; i < TOTAL_NUM_CUBIES; i++)
				for (int j = i + 1; j < TOTAL_NUM_CUBIES; j++)
					result[2] ^= _state[i] > _state[j];
			return result;
		}

		// --> Phase 4: The rest
		return _state;
	}

	bool CubeState::operator<(const CubeState& other_state) const {
		return this->_state < other_state._state;
	}
}