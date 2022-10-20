#pragma once

#include <glfw3.h>

/**
* GLFW display that OpenGL draws on. Also represents the GLFW context.
*/
class Window {

	GLFWwindow* _window;
	int _width;
	int _height;
	const char* _title;

public:

	Window(int width, int height, const char*& title, bool fullScreen);
	~Window();
	void makeCurrentContext();
	void setSwapInterval(int interval);
	void swapBuffers();
	bool running();
	void printGLFWInfo();
	GLFWwindow* getWindow();

private:
	int init(bool fullScreen);
};
