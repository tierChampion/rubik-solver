#include "model.h"

namespace rubik {

	/**
	* Initializes a cubie.
	*
	* @param the coordinates of the cubie in cubie space
	* @param scale of the cubie
	*/
	CubieModel::CubieModel(int x, int y, int z, float scaleFactor) {

		float SCALE_FACTOR_X = (x - 1) * (scaleFactor * 1.25f) + 1;
		float SCALE_FACTOR_Y = (y - 1) * (scaleFactor / 2) + 1;
		float SCALE_FACTOR_Z = (z - 1) * (scaleFactor)+1;

		glm::vec3 scale = glm::vec3(SCALE_FACTOR_X, SCALE_FACTOR_Y, SCALE_FACTOR_Z);

		/*
		* Position is half the size of the current cubie plus half the center cubie,
		* unless the current cubie is the center.
		*/
		_pos = glm::vec3((x - 1) * (CUBIE_SIZE * (SCALE_FACTOR_X) / 2 + CUBIE_SIZE / 2.0f),
			(y - 1) * (CUBIE_SIZE * (SCALE_FACTOR_Y) / 2 + CUBIE_SIZE / 2.0f),
			(z - 1) * (CUBIE_SIZE * (SCALE_FACTOR_Z) / 2 + CUBIE_SIZE / 2.0f));

		_scaleMat = glm::scale(glm::mat4(1.0f), scale);

		_translateMat = glm::translate(glm::mat4(1.0f), _pos);

		_quaternion = glm::quat(glm::vec3(0, 0, 0));

		determineNormals(x, y, z);
	}

	/**
	* Turns the cubie in world space by theta along a given axis in model space.
	*
	* @param theta - angle to turn around the axis
	* @param axis - axis to turn around in model space
	*/
	void CubieModel::turn(float theta, glm::vec3 axis) {

		/* Rotate the axis so it is in world space. */
		axis = glm::normalize(glm::vec3(glm::vec4(axis, 1.0) * glm::toMat4(_quaternion)));

		glm::quat delta = glm::angleAxis(theta, axis);
		_quaternion = glm::normalize(_quaternion * delta);
	}

	/**
	* Updates the normals of the cubie after a turn, such that it can then be turned
	* by it's new sides.
	*/
	void CubieModel::updateNormals() {

		glm::mat4 unscaledModelMat = glm::toMat4(_quaternion) * _translateMat;
		glm::vec3 newPos = glm::vec3(unscaledModelMat[3]);

		/* Position in cubie space. */
		int x = round(newPos[0] / CUBIE_SIZE) + 1;
		int y = round(newPos[1] / CUBIE_SIZE) + 1;
		int z = round(newPos[2] / CUBIE_SIZE) + 1;

		determineNormals(x, y, z);
	}

	/**
	* Determines the normals of the cubie.
	* They can be found by the location of the cubie in the cube.
	*
	* @param the coordinates of the cubie in cubie space
	*/
	void CubieModel::determineNormals(int x, int y, int z) {

		_faceNormals.clear();

		glm::vec3 pos(x, y, z);

		for (int i = 0; i < DIMENSION; i++) {
			if (pos[i] == 1) continue;

			glm::vec3 normal(0.0f, 0.0f, 0.0f);

			normal[i] = (pos[i] == 0) ? -1.0f : 1.0f;

			_faceNormals.push_back(normal);
		}
	}

	/**
	* @return the model matrix composed of the scale, translation and rotation of the model
	*/
	glm::mat4 CubieModel::getModelMat() {
		return glm::toMat4(_quaternion) * _translateMat * _scaleMat;
	}

	/**
	* @return the normals of the side this cubie is a part of
	*/
	std::vector<glm::vec3> CubieModel::getNormals() {
		return _faceNormals;
	}
}