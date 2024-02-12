#pragma once

#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>

static glm::vec2 _deltaPos;
static glm::vec2 _lastPos;
static float _totalScroll = 0;
static bool _activeDrag = false;

/**
 * GLFW mouse controller.
 */
class Mouse
{

public:
	static void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
	static void mouseScrollCallback(GLFWwindow *window, double xoffset, double yoffset);
	static void update(GLFWwindow *window);
	static glm::vec2 getDrag();
	static float getScroll();
};
