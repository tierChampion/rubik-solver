#pragma once

#include <glfw3.h>
#include "../cube/move.h"

static rubik::Move_Type _m = rubik::Move_Type::NONE;

/**
* GLFW Keyboard controller.
*/
class Keyboard {

public:
	static void turningCube_KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static rubik::Move getMove(GLFWwindow* window);
};

/**
* Getter for the last keyboard move
* @param window - GLFW window
*/
inline rubik::Move Keyboard::getMove(GLFWwindow* window) {
	rubik::Move_Type returnValue = _m;
	if (returnValue != rubik::Move_Type::NONE) {

		int modifier = 0;

		// Double turn
		if (glfwGetKey(window, GLFW_KEY_SPACE)) modifier = 1;

		// Inverse turn
		else if (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) ||
			glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)) modifier = 2;

		returnValue = (rubik::Move_Type)(returnValue + modifier);
	}
	// Reset move
	_m = rubik::Move_Type::NONE;

	return rubik::Move(returnValue);
}

/**
* GLFW Callback to update the rubik's cube move.
* @param window - GLFW window
* @param key - typed key
* @param scancode - code of the typed key
* @param action - what happened to the key
*/
inline void Keyboard::turningCube_KeyCallback(
	GLFWwindow* window,
	int key,
	int scancode,
	int action,
	int mods)
{

	if (action == GLFW_PRESS) {

		/* UP */
		if (key == GLFW_KEY_Q) {
			_m = rubik::Move_Type::U1;
		}

		/* DOWN */
		else if (key == GLFW_KEY_A) {
			_m = rubik::Move_Type::D1;
		}

		/* FRONT */
		else if (key == GLFW_KEY_W) {
			_m = rubik::Move_Type::F1;
		}

		/* BACK */
		else if (key == GLFW_KEY_S) {
			_m = rubik::Move_Type::B1;
		}

		/* LEFT */
		else if (key == GLFW_KEY_E) {
			_m = rubik::Move_Type::L1;
		}

		/* RIGHT */
		else if (key == GLFW_KEY_D) {
			_m = rubik::Move_Type::R1;
		}
	}
}
