#pragma once

#include <vector>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#define _USE_MATH_DEFINES

#include <math.h>

namespace rubik {

	/**
	* All the legal moves of the Rubik's Cube
	*/
	enum Move_Type {
		U1 = 0, U2 = 1, U3 = 2,
		D1 = 3, D2 = 4, D3 = 5,
		F1 = 6, F2 = 7, F3 = 8,
		B1 = 9, B2 = 10, B3 = 11,
		L1 = 12, L2 = 13, L3 = 14,
		R1 = 15, R2 = 16, R3 = 17,
		NONE = -1,
	};

	const static unsigned int MOVES_PER_FACE = 3;

	struct Move {
		Move_Type _type;

		Move(int value) {
			_type = (Move_Type)value;
		}

		Move(int face, int turns) {
			_type = (Move_Type)(turns - 1 + face * MOVES_PER_FACE);
		}

		Move(Move_Type t) {
			_type = t;
		}

		/**
		* @return the number of turns of the move.
		*/
		int getTurns() {
			return _type % MOVES_PER_FACE + 1;
		}

		/**
		* @return the face that is affected by the move.
		*/
		int getFace() {
			return _type / MOVES_PER_FACE;
		}

		/**
		* @return the move that negates this move.
		*/
		Move inverse() {
			return Move(_type + 2 - 2 * (_type % MOVES_PER_FACE));
		}

		/**
		* Get the rotation modification of the model.
		* @return the axis to turn around and the angle to turn by
		*/
		glm::vec4 toAxisAngle() {
			glm::vec3 axis(0, 0, 0);

			int face = getFace();
			int turns = getTurns();

			switch (face) {
			case 0: axis[1] = 1; break;
			case 1: axis[1] = -1; break;
			case 2: axis[2] = 1; break;
			case 3: axis[2] = -1; break;
			case 4: axis[0] = -1; break;
			case 5: axis[0] = 1; break;
			}

			float angle = (turns - (4 * (turns == 3))) * M_PI / 2.0f;

			return glm::vec4(axis, -angle);
		}

		/**
		* @param normals - normals of the sides the cubie touches
		* @return if the piece is part of the side that turns
		*/
		bool affectsPiece(std::vector<glm::vec3> normals) {

			int face = getFace();

			for (int i = 0; i < normals.size(); i++) {

				glm::vec3 normal = normals[i];

				if ((face == 0 && normal[1] > 0) ||
					(face == 1 && normal[1] < 0) ||
					(face == 2 && normal[2] > 0) ||
					(face == 3 && normal[2] < 0) ||
					(face == 4 && normal[0] < 0) ||
					(face == 5 && normal[0] > 0)) return true;
			}

			return false;
		}
	};
}
