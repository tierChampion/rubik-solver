#include "cube/solver.h"

#include <iostream>
#include <string>
#include <map>
#include <algorithm>
#include <chrono>
#include <deque>

namespace rubik
{
	struct TKInformation
	{
		// State that was used to reach the current state
		TKMetrics pred;
		// Direction from which the state was reached (10 for unsolved, 01 for solved)
		// and Move used to reach it
		uint8_t directionMove;
	};

	/**
	 * Compute an algorithm to solve the current scrambled state of the cube
	 * by using a mix between Thistlethwaite's and Kociemba's algorithm. Averages around 28 moves.
	 * The idea is to use the metrics of the Kociemba but with the last phase split into two parts:
	 * the first one is similar to the Thistlethwaite while the second phase is the Kociemba's.
	 * @param problem - state of the cube to solve
	 */
	std::queue<Move> thistlethwaiteKociemba(Cube *cube, CubeState problem)
	{
		auto start = std::chrono::steady_clock::now();

		std::deque<Move> solution;
		CubeState currentState = problem;

		// Initialise the goal state to the solved state [0, 1, ..., 19 / 0, 0, ..., 0]
		CubeState goalState;

		Move lastMove = Move(-1);

		int phase = 0;

		std::map<TKMetrics, TKInformation> searchedSpace;

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
			searchedSpace[currentId].directionMove |= 0x80;
			searchedSpace[goalId].directionMove |= 0x40;

			bool finishedPhase = false;

			while (!finishedPhase)
			{
				// State to explore from
				CubeState oldState = q.front();
				q.pop();

				TKMetrics oldId = oldState.thistlethwaiteKociembaId(phase);
				uint8_t &oldDir = searchedSpace[oldId].directionMove;

				// Explore all the legal moves for new states
				for (uint8_t m = 0; m < NUM_POSSIBLE_MOVES && !finishedPhase; m++)
				{
					if (THISTLETHWAITE_MOVES[phase] & (1 << m))
					{
						Move move(m);

						CubeState newState = oldState.applyMove(move);

						TKMetrics newId = newState.thistlethwaiteKociembaId(phase);
						uint8_t &newDir = searchedSpace[newId].directionMove;

						// The new state has already been seen and it has a different direction.
						// This means that the scrambled and solved states are now connected.
						if ((newDir & 0xC0 != 0) && (newDir & 0xC0) != (oldDir & 0xC0))
						{
							// If the state comes from the solved state, invert the moves.
							if ((oldDir & 0x40) == 0x40)
							{
								TKMetrics temp = oldId;
								oldId = newId;
								newId = temp;
								move = move.inverse();
							}

							std::vector<Move> algorithm(1, move);

							// Connect the positive path
							while (oldId != currentId)
							{
								algorithm.insert(algorithm.begin(), Move(searchedSpace[oldId].directionMove & 0x3F));
								oldId = searchedSpace[oldId].pred;
							}

							// Connect the negative path
							while (newId != goalId)
							{
								algorithm.push_back(Move(searchedSpace[newId].directionMove & 0x3F).inverse());
								newId = searchedSpace[newId].pred;
							}

							if (solution.size() > 0)
							{
								currentState = currentState.applyMove(lastMove.inverse());
								algorithm.insert(algorithm.begin(), lastMove);
								solution.pop_back();
							}

							std::queue<Move> optimized = optimizeSolution(algorithm);
							int totalSize = optimized.size();

							// Apply the algorithm to the scrambled state
							for (int i = 0; i < totalSize; i++)
							{
								Move newMove = optimized.front();
								optimized.pop();
								solution.push_back(newMove);
								currentState = currentState.applyMove(newMove);

								// Dont apply last move of the phase to the physical cube.
								// It can sometimes be optimized and remove useless moves.
								if (i != totalSize - 1)
								{
									cube->turnFace(newMove);
								}
								else
								{
									lastMove = newMove;
								}
							}
							searchedSpace.clear();
							finishedPhase = true;
						}

						// State was never seen. Update the tables and set the direction.
						if (!newDir)
						{
							q.push(newState);
							newDir = ((oldDir & 0xC0) | (move.code() & 0x3F));
							searchedSpace[newId].pred = oldId;
						}
					}
				}
			}
			phase++;
		}
		if (solution.size() > 0)
		{
			cube->turnFace(lastMove);
		}

		auto end = std::chrono::steady_clock::now();

		std::chrono::duration<double> duration = end - start;

		std::cout << "Time: " << duration.count() << " seconds" << std::endl;

		return std::queue(solution);
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
			{
				optimisation.push(Move(face, turns));
			}
		}

		return optimisation;
	}
}
