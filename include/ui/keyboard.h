#pragma once

#include <GLFW/glfw3.h>
#include "../cube/move.h"

static rubik::MoveType _m = rubik::MoveType::NONE;

/**
 * GLFW Keyboard controller.
 */
class Keyboard
{
public:
	static void turningCube_KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
	static rubik::Move getMove(GLFWwindow *window);
};
