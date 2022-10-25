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

	/**
	* Calculate the new state after a given move is applied.
	* @param move - move to apply
	*/
	CubeState CubeState::applyMove(int move) const {
		int turns = move % NUM_MOVES_PER_FACE + 1;
		int face = move / NUM_MOVES_PER_FACE;

		std::vector<int> current_state = this->_state;

		while (turns--) {
			std::vector<int> oldState = current_state;

			for (int i = 0; i < 8; i++) {
				int isCorner = (i >= 4);
				// Swap (0 <- 1 <- 2 <- 3 <- 0) for both edges and corners
				int target = AFFECTED_CUBIES[face][i] + isCorner * NUM_EDGES;
				int newIndex = 0;

				if (i % 4 == 3) newIndex = i - 3;
				else newIndex = i + 1;

				int newVal = AFFECTED_CUBIES[face][newIndex] + isCorner * NUM_EDGES;

				int orientationDelta = 0;

				/*
					Edges only change orientation with one pair of move types.
					The two other pair can always replace the edges to their original state,
					but the last pair flips the edge piece.
				*/
				if (i <= 3) orientationDelta = (face == 2 || face == 3);

				// Corners always change except with up or down moves.
				else if (face <= 1) orientationDelta = 0;

				/*
				* Corner 0: +1 (F) +2 (R)
				* Corner 1: +1 (R) +2 (B)
				* Corner 2: +1 (B) +2 (L)
				* Corner 3: +1 (L) +2 (F)
				* Corner 4: +1 (R) +2 (F)
				* Corner 5: +1 (F) +2 (L)
				* Corner 6: +1 (L) +2 (B)
				* Corner 7: +1 (B) +2 (R)
				*/
				else orientationDelta = 2 - (i % 2);



				target = AFFECTED_CUBIES[face][i] + isCorner * NUM_EDGES;
				newVal = AFFECTED_CUBIES[face][(i & 3) == 3 ? i - 3 : i + 1] + isCorner * NUM_EDGES;
				orientationDelta = (i < 4) ? (face > 1 && face < 4) : (face < 2) ? 0 : 2 - (i & 1);

				/*
				Move and change orientation
				*/
				current_state[target] = oldState[newVal];
				current_state[target + TOTAL_NUM_CUBIES] =
					oldState[newVal + TOTAL_NUM_CUBIES] + orientationDelta;

				// Clamp the orientation
				if (turns != 0)
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
				result[0] |= ((_state[e] >= 8) ? 2 : (_state[e] & 1)) << (2 * e);
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