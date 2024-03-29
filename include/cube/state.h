#pragma once

#include <vector>
#include "move.h"

namespace rubik
{
	/**********************************************************************
	 * A cube 'state' is a vector<int> with 40 entries, the first 20
	 * are a permutation of {0,...,19} and describe which cubie is at
	 * a certain position (regarding the input ordering). The first
	 * twelve are for edges, the last eight for corners.
	 *
	 * The precise order for edges is:
	 *		UF, UR, UB, UL, DF, DR, DB, DL, FR, FL, BR, BL
	 *
	 * The precise order for corner is:
	 *		URF, URB, ULB, ULF, DRF, DRB, DLB, DLF
	 *
	 * The last 20 entries are for the orientations, each describing
	 * how often the cubie at a certain position has been turned
	 * counterclockwise away from the correct orientation. Again the
	 * first twelve are edges, the last eight are corners. The values
	 * are 0 or 1 for edges and 0, 1 or 2 for corners.
	 *
	 **********************************************************************/

	/*
	Binary numbers to query the possible moves:
		- If there is a 1, the move is legal for the given phase
		- If there is a 0, the move is illegal for the given phase
	*/
	const unsigned int THISTLETHWAITE_MOVES[] = {
		// 0b111111111111111111, // {U, D, F, B, L, R}
		0b111111111111111111, // 0b111111010010111111, // {U, D, F2, B2, L, R}
		0b010010010010111111, // {U, D, F2, B2, L2, R2}
		0b010010010010111111, // {U, D, F2, B2, L2, R2}
	};

	const unsigned int KOCIEMBA_MOVES[] = {
		0b111111111111111111, // {U, D, F, B, L, R}
		0b010010010010111111, // {U, D, F2, B2, L2, R2}
	};

	/*
	What cubies while be modified by turning the given face.
	4 edges [0-11] and 4 corners [0-7]
	*/
	const int AFFECTED_CUBIES[][8] = {
		{0, 1, 2, 3, 0, 1, 2, 3},	// U
		{4, 7, 6, 5, 4, 5, 6, 7},	// D
		{0, 9, 4, 8, 0, 3, 5, 4},	// F
		{2, 10, 6, 11, 2, 1, 7, 6}, // B
		{3, 11, 7, 9, 3, 2, 6, 5},	// L
		{1, 8, 5, 10, 1, 0, 4, 7},	// R
	};

	const static unsigned int NUM_POSSIBLE_MOVES = 18;
	const static unsigned int NUM_MOVES_PER_FACE = 3;
	const static unsigned int NUM_EDGES = 12;
	const static unsigned int NUM_MIDDLE_EDGES = 4;
	const static unsigned int NUM_CORNERS = 8;
	const static unsigned int NUM_CENTERS = 6;
	const static unsigned int TOTAL_NUM_CUBIES = NUM_EDGES + NUM_CORNERS;

	struct TKMetrics
	{
		uint16_t m1;
		uint16_t m2;
		uint16_t m3;
		uint16_t m4;
	};

	bool operator<(const TKMetrics &ma, const TKMetrics &mb);

	bool operator==(const TKMetrics &ma, const TKMetrics &mb);
	
	bool operator!=(const TKMetrics &ma, const TKMetrics &mb);

	class CubeState
	{
		std::vector<uint8_t> _state;

	public:
		CubeState();
		CubeState(std::vector<uint8_t> &s);
		CubeState applyMove(const Move &move) const;
		TKMetrics thistlethwaiteKociembaId(unsigned int phase) const;
		int size() const;

		bool operator<(const CubeState &other_state) const;
		int operator[](const int i) const;

		friend std::ostream &operator<<(std::ostream &s, const CubeState &state);
	};
}
