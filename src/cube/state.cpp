#include "cube/state.h"

namespace rubik
{
	CubeState::CubeState()
	{
		_state = std::vector<uint8_t>(2 * TOTAL_NUM_CUBIES + NUM_CENTERS, 0);

		for (int i = 0; i < TOTAL_NUM_CUBIES; i++)
		{
			_state[i] = i;
		}
	}

	CubeState::CubeState(std::vector<uint8_t> &s)
	{
		_state = s;
	}

	/**
	 * Calculate the new state after a given move is applied.
	 * @param move - move to apply
	 */
	CubeState CubeState::applyMove(const Move& move) const
	{
		int turns = move.getTurns();
		int face = move.getFace();

		std::vector<uint8_t> current_state = this->_state;

		for (int t = 0; t < turns; t++)
		{
			std::vector<uint8_t> oldState = current_state;

			for (int i = 0; i < 8; i++)
			{
				int isCorner = (i >= 4);

				// Swap (0 <- 1 <- 2 <- 3 <- 0) for both edges and corners
				int target = AFFECTED_CUBIES[face][i] + isCorner * NUM_EDGES;

				int newIndex = 0;
				if (i % 4 == 3)
					newIndex = i - 3;
				else
					newIndex = i + 1;
				int newVal = AFFECTED_CUBIES[face][newIndex] + isCorner * NUM_EDGES;

				int orientationDelta = 0;

				// Edges only change orientation with one pair of move types.
				// The two other pair can always replace the edges to their original state,
				// but the last pair flips the edge piece.
				if (i <= 3)
					orientationDelta = (face == 2 || face == 3);

				// Corners always change except with up or down moves.
				else if (face <= 1)
					orientationDelta = 0;

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
				else
					orientationDelta = 2 - (i % 2);

				/*
				Move and change orientation
				*/
				current_state[target] = oldState[newVal];
				current_state[target + TOTAL_NUM_CUBIES] =
					oldState[newVal + TOTAL_NUM_CUBIES] + orientationDelta;

				// Clamp the orientation
				current_state[target + TOTAL_NUM_CUBIES] %= 2 + isCorner;
			}
		}

		current_state[2 * TOTAL_NUM_CUBIES + face] += turns;
		current_state[2 * TOTAL_NUM_CUBIES + face] %= 4;

		return current_state;
	}

	/**
	 * Compute the metrics for the thistlethwaite-kociemba algorithm depending on the phase.
	 * @param phase - current phase of the algorithm
	 */
	std::vector<uint16_t> CubeState::thistlethwaiteKociembaId(unsigned int phase) const
	{

		// Phase 1: Orientations and middle slice edges
		if (phase == 0)
		{

			// Orientation of the corners
			std::vector<uint16_t> metrics(4, 0);
			for (int e = TOTAL_NUM_CUBIES + NUM_EDGES; e < 2 * TOTAL_NUM_CUBIES; e++)
			{
				metrics[0] *= 3;
				metrics[0] += _state[e];
			}

			// Store a bit of 1 at the position of the edges
			// in the slice between Red(U) and Orange(D) in their solved state
			// Place all the middle edges in their slice
			for (int e = 0; e < NUM_EDGES; e++)
				metrics[1] |= (_state[e] / (NUM_EDGES - NUM_MIDDLE_EDGES)) << e;

			// Orientation of the edges
			for (int e = TOTAL_NUM_CUBIES; e < TOTAL_NUM_CUBIES + NUM_EDGES - 1; e++)
			{
				metrics[2] *= 2;
				metrics[2] += _state[e];
			}

			// Rough orientation of the F, B, L and R centers. Must be a half-turn
			// away from solved
			for (int c = 2; c < NUM_CENTERS; c++)
			{
				metrics[3] *= 2;
				metrics[3] += _state[2 * TOTAL_NUM_CUBIES + c] & 0b1;
			}

			return metrics;
		}

		// Phase 3: Consider the rough position of the edges, the rough position of the corners
		else if (phase == 1)
		{
			std::vector<uint16_t> metrics(3);

			// Store '11' for edges in the middle slice or either '01' or '00' for other edges
			// depending on if they are odd or even.
			// Checks overall edge location
			for (int e = 0; e < NUM_EDGES; e++)
				metrics[0] |= ((_state[e] >= 8) ? 0b11 : ((int)(_state[e] & 0b1)) << (2 * e));

			// Store '1' as the first bit if the corner is on the bottom slice.
			// Store '1' as the third bit if the corner is on the diagonal L->R/F->B
			// Checks if the corner is in the correct slice and on the right diagonal
			for (int c = 0; c < NUM_CORNERS; c++)
				metrics[1] |= ((int)(_state[c + NUM_EDGES] - NUM_EDGES) & 0b101) << (3 * c);

			// Check if the number of permutations between corners is even.
			// From a solved state, if pairs of corners where swapped an odd
			// number of times, this will be a '1' and if the number of times
			// was even, this will be a '0'.
			for (int i = NUM_EDGES; i < TOTAL_NUM_CUBIES; i++)
			{
				for (int j = i + 1; j < TOTAL_NUM_CUBIES; j++)
				{
					metrics[2] ^= (_state[i] > _state[j]);
				}
			}

			return metrics;
		}

		// Phase 4: Consider the positions of each cubies
		std::vector<uint16_t> metrics(4, 0);

		// Corner permutations
		for (int i = NUM_CORNERS - 1; i >= 1; i--)
		{

			for (int j = i - 1; j >= 0; j--)
			{
				metrics[0] += (_state[i + NUM_EDGES] < _state[j + NUM_EDGES]);
			}

			metrics[0] *= i;
		}

		// Top and bottom edge permutations
		for (int i = NUM_EDGES - NUM_MIDDLE_EDGES - 1; i >= 1; i--)
		{

			for (int j = i - 1; j >= 0; j--)
			{
				metrics[1] += (_state[i] < _state[j]);
			}

			metrics[1] *= i;
		}

		// Middle slice permutations
		for (int i = NUM_MIDDLE_EDGES - 1; i >= 1; i--)
		{

			for (int j = i - 1; j >= 0; j--)
			{
				metrics[2] += (_state[i + 8] < _state[j + 8]);
			}

			metrics[2] *= i;
		}

		// Rough orientation of the U and D centers.
		for (int c = 0; c < 2; c++)
		{
			metrics[3] *= 2;
			metrics[3] += _state[2 * TOTAL_NUM_CUBIES + c] & 1;
		}

		return metrics;
	}

	int CubeState::size() const
	{
		return _state.size();
	}

	/**
	 * Comparison operator for use in maps.
	 * @param other_state
	 */
	bool CubeState::operator<(const CubeState &other_state) const
	{
		return this->_state < other_state._state;
	}

	int CubeState::operator[](const int i) const
	{
		return this->_state[i];
	}

	/**
	 * Show the cube state.
	 * @param state - state to show
	 */
	std::ostream &operator<<(std::ostream &s, const CubeState &state)
	{
		for (int i = 0; i < 2 * TOTAL_NUM_CUBIES + 6; i++)
		{
			s << state[i];
		}

		return s << "\n";
	}
}