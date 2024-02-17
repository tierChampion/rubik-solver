#include "cube/solver.h"

#include <iostream>
#include <string>
#include <map>
#include <algorithm>

namespace rubik
{
	/**
	 * Compute an algorithm to solve the current scrambled state of the cube
	 * by using a mix between Thistlethwaite's and Kociemba's algorithm. Averages around 28 moves.
	 * The idea is to use the metrics of the Kociemba but with the last phase split into two parts:
	 * the first one is similar to the Thistlethwaite while the second phase is the Kociemba's.
	 * @param problem - state of the cube to solve
	 */
	std::vector<Move> thistlethwaiteKociemba(CubeState problem)
	{

		// MAKE THINGS WORK WITH THE TKMETRICS

		std::vector<Move> solution;
		CubeState currentState = problem;

		// Initialise the goal state to the solved state [0, 1, ..., 19 / 0, 0, ..., 0]
		CubeState goalState;

		int phase = 0;

		// State allong with the state that was used to reach it
		std::map<TKMetrics, TKMetrics> predecessor;
		// Direction from which the state was reached. Largest bit set to 1 if the state was reached from
		// the scrambled state and before largest bit to 1 if it was reached from the solved state. A direction
		// of 0 means that the state was never reached before.
		// Move that was used to reach the state
		std::map<TKMetrics, uint8_t> direction;

		while (phase < THISTLETHWAITE_KOCIEMBA_PHASE_COUNT)
		{
			TKMetrics currentId = currentState.thistlethwaiteKociembaId(phase),
								  goalId = goalState.thistlethwaiteKociembaId(phase);

			// Skip the phase if already solved
			if (currentId == goalId)
			{
				phase++;
				continue;
			}

			/* BFS QUEUE */
			std::queue<CubeState> q;
			q.push(currentState);
			q.push(goalState);

			/* BFS TABLES */
			direction[currentId] |= 0x80;
			direction[goalId] |= 0x40;

			bool finishedPhase = false;

			while (!finishedPhase)
			{
				// State to explore from
				CubeState oldState = q.front();
				q.pop();

				TKMetrics oldId = oldState.thistlethwaiteKociembaId(phase);
				uint8_t &oldDir = direction[oldId];

				// Explore all the legal moves for new states
				for (uint8_t m = 0; m < NUM_POSSIBLE_MOVES && !finishedPhase; m++)
				{
					if (THISTLETHWAITE_MOVES[phase] & (1 << m))
					{
						Move move(m);

						CubeState newState = oldState.applyMove(move);

						TKMetrics newId = newState.thistlethwaiteKociembaId(phase);
						uint8_t &newDir = direction[newId];

						// The new state has already been seen and it has a different direction.
						// This means that the scrambled and solved states are now connected.
						if ((newDir & 0xC0 != 0) && (newDir & 0xC0) != (oldDir & 0xC0))
						{
							// If the state comes from the solved state, invert the moves.
							if ((oldDir & 0x40) == 0x40)
							{
								// swap(newId, oldId);
								TKMetrics temp = oldId;
								oldId = newId;
								newId = temp;
								move = move.inverse();
							}

							std::vector<Move> algorithm(1, move);

							// Connect the positive path
							while (oldId != currentId)
							{
								// algorithm.insert(algorithm.begin(), lastMove[oldId]);
								algorithm.insert(algorithm.begin(), Move(direction[oldId] & 0x3F));
								oldId = predecessor[oldId];
							}

							// Connect the negative path
							while (newId != goalId)
							{
								// algorithm.push_back(lastMove[newId].inverse());
								algorithm.push_back(Move(direction[newId] & 0x3F).inverse());
								newId = predecessor[newId];
							}

							// Apply the algorithm to the srambled state
							for (int i = 0; i < algorithm.size(); i++)
							{
								solution.push_back(algorithm[i]);
								currentState = currentState.applyMove(algorithm[i]);
							}

							predecessor.clear();
							direction.clear();

							finishedPhase = true;
						}

						// State was never seen. Update the tables and set the direction.
						if (!newDir)
						{
							q.push(newState);
							newDir = ((oldDir & 0xC0) | (move.code() & 0x3F));
							predecessor[newId] = oldId;
						}
					}
				}
			}
			phase++;
		}

		return solution;
	}

	/**
	 * Solve the orientation of centers using a preset algorithm.
	 * @param problem - State to solve the orientation of the centers of.
	 * @return algorithm to reach a center-solved state
	 */
	std::vector<Move> solveCenters(CubeState problem)
	{
		std::vector<Move> solution;

		for (int i = 0; i < NUM_CENTERS; i++)
		{
			if (problem[2 * TOTAL_NUM_CUBIES + i] != 0)
			{
				if (i < 4)
				{
					// (?RL?2R'L') x2
					for (int a = 0; a < 2; a++)
					{
						solution.push_back(Move(i, 1));
						solution.push_back(Move(MoveType::R1));
						solution.push_back(Move(MoveType::L1));
						solution.push_back(Move(i, 2));
						solution.push_back(Move(MoveType::R3));
						solution.push_back(Move(MoveType::L3));
					}
				}
				else
				{
					// (?UD?2U'D') x2
					for (int a = 0; a < 2; a++)
					{
						solution.push_back(Move(i, 1));
						solution.push_back(Move(MoveType::U1));
						solution.push_back(Move(MoveType::D1));
						solution.push_back(Move(i, 2));
						solution.push_back(Move(MoveType::U3));
						solution.push_back(Move(MoveType::D3));
					}
				}
			}
		}

		return solution;
	}

	/**
	 * Clean up the solution and simplify useless moves.
	 * @param solution - series of moves to simplify
	 */
	std::queue<Move> optimizeSolution(std::vector<Move> solution)
	{
		std::queue<Move> optimisation;

		if (solution.empty())
		{
			return optimisation;
		}

		while (solution.size() != 0)
		{
			Move current = solution[0];
			solution.erase(solution.begin());

			int turns = current.getTurns();
			int face = current.getFace();

			int pos = 0;
			while (solution.size() > pos)
			{
				/*
				 * Step through the moves and group consecutive moves that turn the same face.
				 * Also jump over opposite face turns since they don't modify the turn.
				 */
				if (solution[pos].getAxis() == current.getAxis())
				{
					if (face == solution[pos].getFace())
					{
						turns += solution[pos].getTurns();
						solution.erase(solution.begin() + pos);
					}
					else
					{
						pos++;
					}
				}
				else
					break;
			}

			turns %= 4;
			if (turns != 0)
				optimisation.push(Move(face, turns));
		}

		return optimisation;
	}
}
