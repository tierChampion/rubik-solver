#include "ui/keyboard.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>

/**
 * Getter for the last keyboard move
 * @param window - GLFW window
 */
rubik::Move Keyboard::getMove(GLFWwindow *window)
{
	rubik::MoveType returnValue = _m;
	if (returnValue != rubik::MoveType::NONE)
	{

		int modifier = 0;

		// Double turn
		if (glfwGetKey(window, GLFW_KEY_SPACE))
			modifier = 1;

		// Inverse turn
		else if (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) ||
				 glfwGetKey(window, GLFW_KEY_LEFT_SHIFT))
			modifier = 2;

		returnValue = (rubik::MoveType)(returnValue + modifier);
	}
	// Reset move
	_m = rubik::MoveType::NONE;

	return rubik::Move(returnValue);
}

/**
 * GLFW Callback to update the rubik's cube move.
 * @param window - GLFW window
 * @param key - typed key
 * @param scancode - code of the typed key
 * @param action - what happened to the key
 */
void Keyboard::turningCube_KeyCallback(
	GLFWwindow *window,
	int key,
	int scancode,
	int action,
	int mods)
{
	ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);

    ImGuiIO &io = ImGui::GetIO();
    if (io.WantCaptureKeyboard)
        return;

	if (action == GLFW_PRESS)
	{

		/* UP */
		if (key == GLFW_KEY_Q)
		{
			_m = rubik::MoveType::U1;
		}

		/* DOWN */
		else if (key == GLFW_KEY_A)
		{
			_m = rubik::MoveType::D1;
		}

		/* FRONT */
		else if (key == GLFW_KEY_W)
		{
			_m = rubik::MoveType::F1;
		}

		/* BACK */
		else if (key == GLFW_KEY_S)
		{
			_m = rubik::MoveType::B1;
		}

		/* LEFT */
		else if (key == GLFW_KEY_E)
		{
			_m = rubik::MoveType::L1;
		}

		/* RIGHT */
		else if (key == GLFW_KEY_D)
		{
			_m = rubik::MoveType::R1;
		}
	}
}
