#pragma once

#include <iostream>
#include <glew.h>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>
#include "move.h"
#include "../opengl/camera.h"
#include "../opengl/vao.h"
#include <vector>
#include <queue>

#define _USE_MATH_DEFINES

#include <math.h>

namespace rubik {

	const int CUBIE_SIZE = 2;
	const int DIMENSION = 3;
	const float NORMAL_SCALE = 0.0f;
	const float MIRROR_SCALE = 0.55f;

	/**
	* Implementation of the graphic interface of a single cubie.
	* Has the three parts of the model matrix <translation, rotation and scale> as well as
	* the normals of the face the cubie is a part of.
	* There are 26 (27 minus the center cubie, which is useless) cubies for one rubiks cube.
	*/
	class CubieModel {

		glm::vec3 _pos;
		glm::mat4 _scaleMat;
		glm::mat4 _translateMat;
		glm::quat _quaternion;
		std::vector<glm::vec3> _faceNormals;

	public:
		CubieModel(int x, int y, int z, float scaleFactor);
		void turn(float theta, glm::vec3 axis);
		void updateNormals();
		glm::mat4 getModelMat();
		std::vector<glm::vec3> getNormals();

	private:
		void determineNormals(int x, int y, int z);
	};

	/**
	* Implementation of the graphic interface of the whole rubik's cube.
	* Contains all it's cubies as well as parameters to manage rotation animations and its own rotation.
	*/
	class CubeModel {

		glm::quat _quaternion;
		std::queue<Move> _moves;
		std::queue<std::vector<int>> _targets;
		int _steps;
		int _currentStep;
		std::vector<CubieModel> _cubies;

	public:
		CubeModel(bool mirror);
		bool render(Vao vao, int programId);
		void update();
		void turnFace(Move move);
		void turnCube(glm::vec2 delta);

	private:
		void addTargets();
	};
}
