#include "ui/mouse.h"

/**
 * Callback for the mouse buttons events.
 * @param window - GLFW window
 * @param button - button that is modified by the event
 * @param action - action done to the button
 */
void Mouse::mouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{

    if (button == GLFW_MOUSE_BUTTON_1)
    {

        if (action == GLFW_PRESS && !_activeDrag)
        {
            _activeDrag = true;
            double x, y;
            glfwGetCursorPos(window, &x, &y);

            _lastPos = glm::vec2(x, y);
        }
        else if (action == GLFW_RELEASE && _activeDrag)
        {
            _activeDrag = false;
            _deltaPos = glm::vec2(0);
        }
    }
}

void Mouse::mouseScrollCallback(GLFWwindow *window, double xOffset, double yOffset)
{
    _totalScroll += yOffset;
}

/**
 * Update the position of the mouse.
 * @param window - GLFW window
 */
void Mouse::update(GLFWwindow *window)
{
    if (!_activeDrag)
        return;

    double x = 0, y = 0;
    glfwGetCursorPos(window, &x, &y);

    glm::vec2 newPos(x, y);
    _deltaPos = newPos - _lastPos;
    _lastPos = newPos;
}

/**
 * Getter for the distance change between frames.
 */
glm::vec2 Mouse::getDrag()
{
    return _deltaPos;
}

/**
 * Getter for the scroll change between frames.
 */
float Mouse::getScroll()
{
    float ret = _totalScroll;
    _totalScroll = 0;

    return ret;
}
