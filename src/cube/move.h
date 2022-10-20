#pragma once

#include <vector>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#define _USE_MATH_DEFINES

#include <math.h>

namespace rubik {
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
		Move_Type type;

		Move(int value) {
			type = (Move_Type)value;
		}

		Move(int face, int turns) {
			type = (Move_Type)(turns - 1 + face * MOVES_PER_FACE);
		}

		Move(Move_Type t) {
			type = t;
		}

		int getTurns() {
			return type % MOVES_PER_FACE + 1;
		}

		int getFace() {
			return type / MOVES_PER_FACE;
		}

		Move inverse() {
			return Move(type + 2 - 2 * (type % MOVES_PER_FACE));
		}

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
