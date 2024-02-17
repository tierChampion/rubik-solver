#pragma once

#include <iostream>
#include <vector>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#define _USE_MATH_DEFINES

#include <math.h>

namespace rubik
{

	enum MoveAxis
	{
		Y = 0,
		Z = 1,
		X = 2,
	};

	/**
	 * All the legal moves of the Rubik's Cube
	 */
	enum MoveType
	{
		U1 = 0,
		U2 = 1,
		U3 = 2,
		D1 = 3,
		D2 = 4,
		D3 = 5,
		F1 = 6,
		F2 = 7,
		F3 = 8,
		B1 = 9,
		B2 = 10,
		B3 = 11,
		L1 = 12,
		L2 = 13,
		L3 = 14,
		R1 = 15,
		R2 = 16,
		R3 = 17,
		NONE = -1,
	};

	const static unsigned int MOVES_PER_FACE = 3;

	struct Move
	{
		MoveType _type;

		Move();
		Move(int value);
		Move(int face, int turns);
		Move(MoveType t);

		uint8_t code() const;

		int getTurns() const;

		int getFace() const;

		Move inverse() const;

		MoveAxis getAxis() const;

		glm::vec4 toAxisAngle() const;

		bool affectsPiece(const std::vector<glm::vec3> normals) const;

		friend std::ostream &operator<<(std::ostream &s, const Move &move);
	};
}
