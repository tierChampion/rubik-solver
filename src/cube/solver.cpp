#include "solver.h"

namespace rubik {

	vector<Move> getSolution(CubeState problem) {
		// --> Define the goal
		string goal[] = { "UF", "UR", "UB", "UL", "DF", "DR", "DB", "DL", "FR", "FL", "BR", "BL",
		"UFR", "URB", "UBL", "ULF", "DRF", "DFL", "DLB", "DBR" };

		// --> Define solution
		vector<Move> solution;

		// --> Prepare current (start) and goal state
		CubeState currentState = problem, goalState;

		// --> Define phase
		int phase = 0;

		// --> Thistlethwaite
		while (++phase < 5) {

			// --> Compute ids, skip phase if equal
			std::vector<int> currentId = currentState.id(phase), goalId = goalState.id(phase); // TODO cubestate.id
			if (currentId == goalId)
				continue;

			// --> Initialize the BFS queue
			queue<CubeState> q; // TODO cubestate
			q.push(currentState);
			q.push(goalState);

			// --> Initialize the BFS tables
			map<std::vector<int>, std::vector<int>> predecessor; // TODO cubestate, cubestate
			map<CubeState, int> direction, lastMove; // TODO cubestate, int
			direction[currentId] = 1;
			direction[goalId] = 2;

			while (1) {

				// --> Get state from queue, compute its Id and get its direction
				CubeState oldState = q.front();
				q.pop();
				std::vector<int> oldId = oldState.id(phase);
				int& oldDir = direction[oldId];

				// --> Apply all applicable moves to it and handle the new state
				for (int move = 0; move < NUM_POSSIBLE_MOVES; move++) {
					if (APPLICABLE_MOVES[phase] & (1 << move)) {

						// --> Apply the move
						CubeState newState = oldState.applyMove(move); // TODO oldstate.applymove
						std::vector<int> newId = newState.id(phase);
						int& newDir = direction[newId];

						// --> Have we already found this Id? Can we connect this branch to another one?
						if (newDir && newDir != oldDir) {

							// --> Make oldId represent the forwards and newId the backwards search state
							if (oldDir > 1) {
								swap(newId, oldId);
								move = inverse(move);
							}

							// --> Reconstruct the connecting algorithm
							std::vector<int> algorithm(1, move);
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
								currentState = currentState.applyMove(algorithm[i]);
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

