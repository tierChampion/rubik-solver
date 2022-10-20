#pragma once

#include "glfw3.h"
#include "glm/vec2.hpp"

static glm::vec2 _delta;
static glm::vec2 _lastPos;
static bool _activeDrag;

/**
* GLFW mouse controller.
*/
class Mouse {

public:

	inline static void mousebuttonCallback(GLFWwindow* window, int button, int action, int mods);
	inline static void update(GLFWwindow* window);
	inline static glm::vec2 getDelta();

};

/**
* Callback for the mouse buttons events.
* @param window - GLFW window
* @param button - button that is modified by the event
* @param action - action done to the button
*/
inline void Mouse::mousebuttonCallback(GLFWwindow* window, int button, int action, int mods) {

	if (button == GLFW_MOUSE_BUTTON_1) {

		if (action == GLFW_PRESS && !_activeDrag) {
			_activeDrag = true;
			double x, y;
			glfwGetCursorPos(window, &x, &y);

			if (x != NULL && y != NULL) {
				_lastPos = glm::vec2(x, y);
			}

		}
		else if (action == GLFW_RELEASE && _activeDrag) {
			_activeDrag = false;
			_delta = glm::vec2(0);
		}

	}

}

/**
* Update the position of the mouse.
* @param window - GLFW window
*/
inline void Mouse::update(GLFWwindow* window) {
	if (!_activeDrag) return;

	double x = 0, y = 0;
	glfwGetCursorPos(window, &x, &y);

	if (x != NULL && y != NULL) {
		glm::vec2 newPos(x, y);
		_delta = newPos - _lastPos;
		_lastPos = newPos;
	}
}

/**
* Getter for the distance change between frames.
*/
inline glm::vec2 Mouse::getDelta() {
	return _delta;
}
