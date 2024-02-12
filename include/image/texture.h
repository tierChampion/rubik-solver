#pragma once

#include "stb_image.h"
#include <GL/glew.h>

/**
 * OpenGL image texture
 */
class Texture
{
	int _width;
	int _height;
	int _bpp;
	GLuint _id;
	unsigned char *_pixels;

public:
	Texture(const char *filepath);
	bool load(const char *filepath);
	void passToOpenGL();
	void createGLSLTexture();
	void bind(unsigned int textureUnit);
	int getWidth();
	int getHeight();
	int getComponents();
	unsigned char *getPixels();
};

inline int Texture::getWidth()
{
	return _width;
}

inline int Texture::getHeight()
{
	return _height;
}

inline int Texture::getComponents()
{
	return _bpp;
}

inline unsigned char *Texture::getPixels()
{
	return _pixels;
}
