#include "state.h"

namespace rubik {
	CubeState::CubeState() {
		for (int i = 0; i < 2; i++) {
			for (int j = 0; j < 20; j++) {
				state.push_back(j * (i == 0));
			}
		}
	}

	CubeState::CubeState(rubikState s) {

		state = s;
	}


	CubeState CubeState::applyMove(Move move) {
		int turns = move.getTurns();
		int face = move.getFace();

		rubikState returnState = state;

		while (turns--) {
			rubikState oldState = returnState;

			for (int i = 0; i < NUM_CORNERS; i++) {
				int isCorner = i > 3;
				int target = AFFECTED_CUBIES[face][i] + isCorner * NUM_EDGES;
				int killer = AFFECTED_CUBIES[face][(i & 3) == 3 ? i - 3 : i + 1] + isCorner * NUM_EDGES;
				int orientationDelta = (i < 4) ? (face > 1 && face < 4) : (face < 2) ? 0 : 2 - (i & 1);

				/*
				Move and change orientation
				*/
				returnState[target] = oldState[killer];
				returnState[target + TOTAL_NUM_CUBIES] = oldState[killer + TOTAL_NUM_CUBIES] + orientationDelta;

				if (!turns)
					returnState[target + TOTAL_NUM_CUBIES] %= 2 + isCorner;
			}
		}

		return CubeState(returnState);
	}


	rubikState CubeState::applyMove(int move, rubikState state) {
		int turns = move % NUM_MOVES_PER_FACE + 1;
		int face = move / NUM_MOVES_PER_FACE;

		while (turns--) {
			rubikState oldState = state;

			for (int i = 0; i < NUM_CORNERS; i++) {
				int isCorner = i > 3;
				int target = AFFECTED_CUBIES[face][i] + isCorner * NUM_EDGES;
				int killer = AFFECTED_CUBIES[face][(i & 3) == 3 ? i - 3 : i + 1] + isCorner * NUM_EDGES;
				int orientationDelta = (i < 4) ? (face > 1 && face < 4) : (face < 2) ? 0 : 2 - (i & 1);

				/*
				Move and change orientation
				*/
				state[target] = oldState[killer];
				state[target + TOTAL_NUM_CUBIES] = oldState[killer + TOTAL_NUM_CUBIES] + orientationDelta;

				if (!turns)
					state[target + TOTAL_NUM_CUBIES] %= 2 + isCorner;
			}
		}

		this->state = state;
		return state;
	}

	rubikState CubeState::id(unsigned int phase) {
		// --> Phase 1: Edge orientations (orientation of the 12 first cubies)
		if (phase < 2)
			return rubikState(state.begin() + TOTAL_NUM_CUBIES, state.begin() + TOTAL_NUM_CUBIES + NUM_EDGES);

		// --> Phase 2: Corner orientations, E slice edges 
		// (12 edges as int in index 0 and orientation of corners in rest)
		if (phase < 3) {
			rubikState result(state.begin() + TOTAL_NUM_CUBIES + NUM_EDGES - 1, state.begin() + 40);
			for (int e = 0; e < NUM_EDGES; e++)
				result[0] |= (state[e] / 8) << e;

			return result;
		}

		// --> Phase 3: Edge slices M and S, corner tetrads, overall parity
		if (phase < 4) {
			rubikState result(3);
			for (int e = 0; e < NUM_EDGES; e++)
				result[0] |= ((state[e] > 7) ? 2 : (state[e] & 1)) << (2 * e);
			for (int c = 0; c < NUM_CORNERS; c++)
				result[1] |= ((state[c + NUM_EDGES] - NUM_EDGES) & 5) << (3 * c);
			for (int i = NUM_EDGES; i < TOTAL_NUM_CUBIES; i++)
				for (int j = i + 1; j < TOTAL_NUM_CUBIES; j++)
					result[2] ^= state[i] > state[j];
			return result;
		}

		// --> Phase 4: The rest
		return state;
	}
}