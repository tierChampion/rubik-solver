#include "image/texture.h"

#include "helper_gl.h"

/**
 * Create an OpenGL texture using the given image file
 * @param filepath - path of the image file to load in the texture
 */
Texture::Texture(const char *filepath) : _width(0), _height(0), _bpp(0), _pixels(nullptr)
{
	bool status = load(filepath);
	if (!status)
	{
		printf("Could not load the texture! ");
	}
}

Texture::Texture() : _width(0), _height(0), _bpp(0), _pixels(nullptr)
{}

/**
 * Read the file and set the parameters of the texture
 * @param filepath - path of the image file to read
 */
bool Texture::load(const char *filepath)
{
	_pixels = stbi_load(filepath, &_width, &_height, &_bpp, 0);
	if (!_pixels)
	{
		return false;
	}
	return true;
}

/**
 * Provide OpenGL with the image data for the texture.
 */
void Texture::passToOpenGL()
{
	glGenTextures(1, &_id);
	createGLSLTexture();
	/* Free memory associated with the pixels. */
	stbi_image_free(_pixels);

	SDK_CHECK_ERROR_GL();
}

/**
 * Generate an OpenGL texture for the texture data.
 */
void Texture::createGLSLTexture()
{
	glBindTexture(GL_TEXTURE_2D, _id);

	// Set the parameters. Repeat the texture when OOB and linear antialiasing
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _width, _height, 0, GL_RGB, GL_UNSIGNED_BYTE, _pixels);
	glBindTexture(GL_TEXTURE_2D, 0);
}

/**
 * Bind the texture to a specific unit for use.
 * @param textureUnit - unit to bind the texture to
 */
void Texture::bind(unsigned int textureUnit)
{
	glActiveTexture(GL_TEXTURE0 + textureUnit);
	glBindTexture(GL_TEXTURE_2D, _id);
}
