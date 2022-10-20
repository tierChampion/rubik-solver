#include "model.h";

namespace rubik {
	/**
	* Initializes the model of the rubiks cube and its 26 cubies.
	* Can be the regular cube or the mirror cube.
	* The mirror cube has different scales for each cubies depending on its position.
	*
	* @param mirror -> whether the cube is a mirror cube or a regular rubiks cube
	*/
	CubeModel::CubeModel(bool mirror) {

		float scale = mirror ? MIRROR_SCALE : NORMAL_SCALE;

		for (int x = 0; x < DIMENSION; x++) {
			for (int y = 0; y < DIMENSION; y++) {
				for (int z = 0; z < DIMENSION; z++) {
					if (x != 1 || y != 1 || z != 1) cubies.push_back(CubieModel(x, y, z, scale));

				}
			}
		}

		quaternion = glm::quat(glm::vec3(0, 0, 0));
		steps = 10;
		currentStep = 0;
	}

	/**
	* Renders the cube with the given vao and shader program.
	*
	* @param vao -> Vertex Array Buffer for the shape to render
	* @param programId -> Shader program to use
	*/
	bool CubeModel::render(Vao vao, int programId) {

		vao.bind();

		GLint mLocation = glGetUniformLocation(programId, "model");

		for (CubieModel cubie : cubies) {

			glm::mat4 m = glm::toMat4(quaternion) * cubie.getModelMat();

			glUniformMatrix4fv(mLocation, 1, GL_FALSE, &m[0][0]);

			glDrawArrays(GL_TRIANGLES, 0, vao.getTriCount());
		}

		vao.unbind();

		return true;
	}

	/*
		Updates the cube by doing a fraction of the last move in the queue.
	*/
	void CubeModel::update() {

		if (moves.size() > 0) {

			glm::vec4 axisAngle = moves.front().toAxisAngle();

			glm::vec3 axis = glm::vec3(axisAngle[0], axisAngle[1], axisAngle[2]);
			float theta = axisAngle[3] / steps;

			for (int target : targets.front()) {
				cubies[target].turn(theta, axis);
			}

			currentStep++;
			if (currentStep == steps) {
				currentStep = 0;
				moves.pop();

				for (int target : targets.front()) {
					cubies[target].updateNormals();
				}

				targets.pop();

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

		moves.push(move);

		if (moves.size() == 1) {
			addTargets();
		}
	}

	/**
	* Turns the whole cube.
	*
	* @param delta -> variation in x and y of the orientation
	*/
	void CubeModel::turnCube(glm::vec2 delta) {

		glm::vec3 upAxis(glm::vec4(0, 1, 0, 0) * glm::toMat4(quaternion));
		glm::vec3 rightAxis(glm::vec4(1, 0, 0, 0) * glm::toMat4(quaternion));

		glm::quat yRotation = glm::angleAxis(glm::radians(delta[0]), upAxis);
		glm::quat xRotation = glm::angleAxis(glm::radians(delta[1]), rightAxis);

		quaternion = glm::normalize(quaternion * (xRotation * yRotation));

	}

	/*
		Finds the cubies affected by the last move in the queue.
	*/
	void CubeModel::addTargets() {

		if (moves.size() > 0) {

			Move move = moves.front();

			std::vector<int> moveTargets;

			for (int c = 0; c < cubies.size(); c++) {

				if (move.affectsPiece(cubies[c].getNormals())) moveTargets.push_back(c);
			}

			targets.push(moveTargets);
		}
	}

}
