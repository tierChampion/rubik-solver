#include <iostream>

#include "ui/window.h"

/**
* @param width - width in pixels of the window
* @param height - height in pixels of the window
* @param title - title of the window created
* @param fullScreen - whether the window should be in fullscreen mode
*/
Window::Window(int width, int height, float aspect_ratio, const char*& title, bool fullScreen) :
	_width(width), _height(height), _title(title)
{
	if (init(aspect_ratio, fullScreen) != 0) {
		std::cerr << "Problem while initialising GLFW window." << std::endl;
	}
}

Window::~Window() {
	glfwDestroyWindow(_window);
	glfwTerminate();
}

GLFWwindow* Window::getWindow() {
	return _window;
}

/**
* Make this window the current GLFW context.
*/
void Window::makeCurrentContext() {
	glfwMakeContextCurrent(_window);
}

/**
* Set the visual frame rate of the window.
*/
void Window::setSwapInterval(int interval) {
	glfwSwapInterval(interval);
}

/**
* Change frame.
*/
void Window::swapBuffers() {
	glfwSwapBuffers(_window);
}

/**
* @returns if the window is openned.
*/
bool Window::running() {
	return !glfwWindowShouldClose(_window);
}

/**
* Show all the information of the window and the context.
*/
void Window::printGLFWInfo() {
	int p = glfwGetWindowAttrib(_window, GLFW_OPENGL_PROFILE);
	std::string version = glfwGetVersionString();
	std::string openglProfile = "";
	if (p == GLFW_OPENGL_COMPAT_PROFILE) {
		openglProfile = "OpenGL Compatibility Profile";
	}
	else if (p == GLFW_OPENGL_CORE_PROFILE) {
		openglProfile = "OpenGL Core Profile";
	}
	printf("GLFW: %s \n", version.c_str());
	printf("GLFW: %s %i \n", openglProfile.c_str(), p);
}

/**
* Initialise the GLFW window with the proper parameters.
*/
int Window::init(float aspect_ratio, bool fullScreen) {

	if (!fullScreen) {
		_window = glfwCreateWindow(_width, _height, _title, NULL, NULL);
	}
	else {
		// Resize the window for fullscreen, keeping the aspect ratio
		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		glfwGetMonitorWorkarea(monitor, NULL, NULL, NULL, &_height);
		_width = aspect_ratio * _height;
		_window = glfwCreateWindow(_width, _height, _title, monitor, NULL);
	}

	if (_window == nullptr) {
		glfwTerminate();
		std::cerr << "Problem while creating GLFW window" << std::endl;
		exit(EXIT_FAILURE);
	}
	return 0;
}