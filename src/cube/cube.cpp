#include "cube.h"

namespace rubik {

	Cube::Cube(bool mirror) : model(CubeModel(mirror)), state() {

	}

	void Cube::update() {
		model.update();
	}

	void Cube::render(Vao vao, int programId) {

		model.render(vao, programId);

	}

	void Cube::turnFace(Move move) {

		model.turnFace(move);
		state = state.applyMove(move.type, state.getState());

	}

	void Cube::turnCube(glm::vec2 delta) {

		model.turnCube(delta);

	}

	void Cube::solve() {

		queue<Move> solution = optimizeSolution(getSolution(state));

		printf("Solution of %d moves found! \n", solution.size());

		while (solution.size() > 0) {
			Move move = solution.front();
			turnFace(move);
			solution.pop();
		}

	}

	void Cube::mix() {

		/* Mix between 10 and 30 moves. */
		int lengthOfMix = rand() % 20 + 20;

		for (int i = 0; i < lengthOfMix; i++) {
			Move randomMove(rand() % NUM_POSSIBLE_MOVES);

			turnFace(randomMove);
		}
	}

}