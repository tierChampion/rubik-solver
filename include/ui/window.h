#pragma once

#include <GLFW/glfw3.h>

/**
 * GLFW display that OpenGL draws on. Also represents the GLFW context.
 */
class GameWindow
{
	GLFWwindow *_window;
	int _width;
	int _height;
	const char *_title;

public:
	GameWindow(int width, int height, float aspect_ratio, const char *&title, bool fullScreen);
	~GameWindow();
	void makeCurrentContext();
	void setSwapInterval(int interval);
	void swapBuffers();
	bool running();
	void printGLFWInfo();
	GLFWwindow *getWindow();

private:
	int init(float aspect_ratio, bool fullScreen);
};

