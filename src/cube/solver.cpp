#include "solver.h"

namespace rubik {

	rubikState applyMove(int move, rubikState state) {
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

		return state;
	}

	rubikState id(rubikState state, int phase) {

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

	vector<Move> getSolution(CubeState problem) {
		// --> Define the goal
		string goal[] = { "UF", "UR", "UB", "UL", "DF", "DR", "DB", "DL", "FR", "FL", "BR", "BL",
		"UFR", "URB", "UBL", "ULF", "DRF", "DFL", "DLB", "DBR" };

		// --> Define solution
		vector<Move> solution;

		// --> Prepare current (start) and goal state
		rubikState currentState = problem.getState(), goalState(40);
		for (int i = 0; i < TOTAL_NUM_CUBIES; i++) {

			// --> Goal state
			goalState[i] = i;
		}

		// --> Define phase
		int phase = 0;

		// --> Thistlethwaite
		while (++phase < 5) {

			// --> Compute ids, skip phase if equal
			rubikState currentId = id(currentState, phase), goalId = id(goalState, phase);
			if (currentId == goalId)
				continue;

			// --> Initialize the BFS queue
			queue<rubikState> q;
			q.push(currentState);
			q.push(goalState);

			// --> Initialize the BFS tables
			map<rubikState, rubikState> predecessor;
			map<rubikState, int> direction, lastMove;
			direction[currentId] = 1;
			direction[goalId] = 2;

			while (1) {

				// --> Get state from queue, compute its Id and get its direction
				rubikState oldState = q.front();
				q.pop();
				rubikState oldId = id(oldState, phase);
				int& oldDir = direction[oldId];

				// --> Apply all applicable moves to it and handle the new state
				for (int move = 0; move < NUM_POSSIBLE_MOVES; move++) {
					if (APPLICABLE_MOVES[phase] & (1 << move)) {

						// --> Apply the move
						rubikState newState = applyMove(move, oldState);
						rubikState newId = id(newState, phase);
						int& newDir = direction[newId];

						// --> Have we already found this Id? Can we connect this branch to another one?
						if (newDir && newDir != oldDir) {

							// --> Make oldId represent the forwards and newId the backwards search state
							if (oldDir > 1) {
								swap(newId, oldId);
								move = inverse(move);
							}

							// --> Reconstruct the connecting algorithm
							rubikState algorithm(1, move);
							while (oldId != currentId) {
								algorithm.insert(algorithm.begin(), lastMove[oldId]);
								oldId = predecessor[oldId];
							}
							while (newId != goalId) {
								algorithm.push_back(inverse(lastMove[newId]));
								newId = predecessor[newId];
							}

							// --> Print and apply the algorithm
							for (int i = 0; i < (int)algorithm.size(); i++) {
								solution.push_back(algorithm[i]);
								currentState = applyMove(algorithm[i], currentState);
							}

							// --> Jump to the next phase
							goto nextPhase;

						}

						if (!newDir) {
							q.push(newState);
							newDir = oldDir;
							lastMove[newId] = move;
							predecessor[newId] = oldId;
						}
					}
				}
			}
		nextPhase:;
		}

		return solution;

	}

	queue<Move> optimizeSolution(vector<Move> solution) {

		queue<Move> optimisation;

		if (solution.size() == 0) {
			return optimisation;
		}

		int i;

		for (i = 0; i < solution.size() - 1; i++) {

			if (solution[i].getFace() == solution[i + 1].getFace()) {
				int turns = (solution[i].getTurns() + solution[i + 1].getTurns()) % 4;

				if (turns != 0) {
					optimisation.push(Move(solution[i].getFace(), turns));
				}
				i++;

			}
			else {
				optimisation.push(solution[i]);
			}

		}

		if (i < solution.size()) {
			optimisation.push(solution[solution.size() - 1]);
		}
		return optimisation;

	}

}

