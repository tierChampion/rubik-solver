#include "cube.h"

namespace rubik {

	Cube::Cube(bool mirror) : _model(CubeModel(mirror)), _state() {}

	/**
	* Update the orientation of the cube.
	*/
	void Cube::update() {
		_model.update();
	}

	/**
	* Render the model with the proper shader program.
	* @param vao - model to use
	* @param programId - id of the program to use
	*/
	void Cube::render(Vao vao, int programId) {

		_model.render(vao, programId);
	}

	/**
	* Execute a move on the cube.
	* @param move - the move to execute
	*/
	void Cube::turnFace(Move move) {

		_model.turnFace(move);
		_state = _state.applyMove(move._type);

	}

	/**
	* Turn the whole cube by a given amount in each directions.
	* @param delta - angle variation in the x and y axis
	*/
	void Cube::turnCube(glm::vec2 delta) {

		_model.turnCube(delta);

	}

	/**
	* Find the solution of the current position and execute it.
	*/
	void Cube::solve() {

		queue<Move> solution = optimizeSolution(getSolution(_state));

		printf("<SOLUTION> %d moves: ", solution.size());

		while (solution.size() > 0) {
			Move move = solution.front();
			std::cout << move << " ";
			turnFace(move);
			solution.pop();
		}

		std::cout << std::endl;
	}

	/**
	* Execute a random sequence of moves to scrabble the cube.
	*/
	void Cube::mix() {

		/* Mix between 10 and 30 moves. */
		int lengthOfMix = rand() % 20 + 20;

		for (int i = 0; i < lengthOfMix; i++) {
			Move randomMove(rand() % NUM_POSSIBLE_MOVES);

			turnFace(randomMove);
		}
	}
}