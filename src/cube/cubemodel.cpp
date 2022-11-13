#include "model.h";

namespace rubik {
	/**
	* Initializes the model of the rubiks cube and its 26 cubies.
	* Can be the regular cube or the mirror cube.
	* The mirror cube has different scales for each cubies depending on its position.
	*
	* @param mirror -> whether the cube is a mirror cube or a regular rubiks cube
	*/
	CubeModel::CubeModel(bool mirror, bool splitted) {

		float scale = mirror ? MIRROR_SCALE : NORMAL_SCALE;

		for (int x = 0; x < DIMENSION; x++) {
			for (int y = 0; y < DIMENSION; y++) {
				for (int z = 0; z < DIMENSION; z++) {
					if (x != 1 || y != 1 || z != 1) _cubies.push_back(CubieModel(x, y, z, scale, splitted));

				}
			}
		}

		_quaternion = glm::quat(glm::vec3(0, 0, 0));
		_steps = 10;
		_currentStep = 0;
		_splitted = splitted;
	}

	/**
	* Renders the cube with the given vao and shader program.
	*
	* @param vao -> Vertex Array Buffer for the shape to render
	* @param programId -> Shader program to use
	*/
	bool CubeModel::render(const std::vector<Vao>& vaos, int programId) {

		GLint mLocation = glGetUniformLocation(programId, "model");

		int index = 0;

		for (CubieModel cubie : _cubies) {

			vaos[index * _splitted].bind();

			glm::mat4 m = glm::toMat4(_quaternion) * cubie.getModelMat();

			glUniformMatrix4fv(mLocation, 1, GL_FALSE, &m[0][0]);

			glDrawArrays(GL_TRIANGLES, 0, vaos[index * _splitted].getTriCount());

			vaos[index * _splitted].unbind();

			index++;
		}

		return true;
	}

	/*
		Updates the cube by doing a fraction of the last move in the queue.
	*/
	void CubeModel::update() {

		if (_moves.size() > 0) {

			glm::vec4 axisAngle = _moves.front().toAxisAngle();

			glm::vec3 axis = glm::vec3(axisAngle[0], axisAngle[1], axisAngle[2]);
			float theta = axisAngle[3] / _steps;

			for (int target : _targets.front()) {
				_cubies[target].turn(theta, axis);
			}

			_currentStep++;
			if (_currentStep == _steps) {
				_currentStep = 0;
				_moves.pop();

				for (int target : _targets.front()) {
					_cubies[target].updateNormals();
				}

				_targets.pop();

				addTargets();
			}
		}
	}

	/**
	* Add move to the queue of moves.
	*
	* @param move -> move to add to the queue
	*/
	void CubeModel::turnFace(Move move) {

		_moves.push(move);

		if (_moves.size() == 1) {
			addTargets();
		}
	}

	/**
	* Turns the whole cube.
	*
	* @param delta -> variation in x and y of the orientation
	*/
	void CubeModel::turnCube(glm::vec2 delta) {

		glm::vec3 upAxis(glm::vec4(0, 1, 0, 0) * glm::toMat4(_quaternion));
		glm::vec3 rightAxis(glm::vec4(1, 0, 0, 0) * glm::toMat4(_quaternion));

		glm::quat yRotation = glm::angleAxis(glm::radians(delta[0]), upAxis);
		glm::quat xRotation = glm::angleAxis(glm::radians(delta[1]), rightAxis);

		_quaternion = glm::normalize(_quaternion * (xRotation * yRotation));

	}

	/**
	* Finds the cubies affected by the last move in the queue.
	*/
	void CubeModel::addTargets() {

		if (_moves.size() > 0) {

			Move move = _moves.front();

			std::vector<int> moveTargets;

			for (int c = 0; c < _cubies.size(); c++) {

				if (move.affectsPiece(_cubies[c].getNormals())) moveTargets.push_back(c);
			}

			_targets.push(moveTargets);
		}
	}

}
