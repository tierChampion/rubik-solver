#include "cube/cube.h"

namespace rubik
{

	Cube::Cube(CubeType type) : _model(CubeModel(type)), _state(), _type(type),
								_centerOrientation(type == CubeType::SPLIT) {}

	Cube::Cube() : Cube(CubeType::REGULAR) {}

	/**
	 * Update the orientation of the cube.
	 */
	void Cube::update()
	{
		_model.update();
	}

	/**
	 * Render the model with the proper shader program.
	 * @param vao - model to use
	 * @param programId - id of the program to use
	 */
	void Cube::render(const std::vector<Vao> &vaos, int programId)
	{

		_model.render(vaos, programId);
	}

	/**
	 * Execute a move on the cube.
	 * @param move - the move to execute
	 */
	void Cube::turnFace(Move move)
	{
		_model.turnFace(move);
		_state = _state.applyMove(move._type);
	}

	/**
	 * Turn the whole cube by a given amount in each directions.
	 * @param delta - angle variation in the x and y axis
	 */
	void Cube::turnCube(glm::vec2 delta)
	{
		_model.turnCube(delta);
	}

	/**
	 * Find the solution of the current position and execute it.
	 */
	void Cube::solve()
	{
		_solving = true;

		std::queue<Move> solution = optimizeSolution(thistlethwaiteKociemba(_state));

		// Show the solution in the terminal
		if (!solution.empty())
		{
			std::cout << "<SOLUTION> " << solution.size() << " moves: ";

			while (solution.size() > 0)
			{
				Move move = solution.front();
				std::cout << move << " ";
				turnFace(move);
				solution.pop();
			}

			std::cout << std::endl;
		}

		if (_centerOrientation)
		{
			std::vector<Move> extra = solveCenters(_state);

			if (!extra.empty())
			{
				std::cout << "EXTRA CENTER ORIENTATIONS> " << extra.size() << " moves" << std::endl;

				for (int m = 0; m < extra.size(); m++)
				{
					turnFace(extra[m]);
				}
			}
		}

		_solving = false;
	}

	bool Cube::isSolving()
	{
		return _solving;
	}

	/**
	 * Execute a random sequence of moves to scrabble the cube.
	 */
	void Cube::mix()
	{
		/* Mix between 40 and 60 moves. */
		int lengthOfMix = rand() % 20 + 40;

		for (int i = 0; i < lengthOfMix; i++)
		{
			Move randomMove(rand() % NUM_POSSIBLE_MOVES);

			turnFace(randomMove);
		}
	}

	void Cube::changeType(CubeType newType)
	{
		_model.changeType(newType);
		_centerOrientation = newType == CubeType::SPLIT;
	}

	/**
	 * Show the cube state for debugging purposes
	 * @param cube - cube to show the state of
	 */
	std::ostream &operator<<(std::ostream &s, const Cube &cube)
	{
		return s << cube._state;
	}
}