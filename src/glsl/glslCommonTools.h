#pragma once

#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif

#include <glew.h>
#include <cstdio>
#include <string>
#include <fstream>
#include <exception>

/**
* Helper string for easy conversion to GLChar
*/
struct ShaderStringHelper {
	const char* p;
	ShaderStringHelper(const std::string& s) : p(s.c_str()) {}
	operator const char** () { return &p; }
};

/**
* Function to load text from file.
* @param fileName - file to read
* @return string containing the text in the file
*/
inline static std::string loadFileToString(const char* fileName) {
	std::ifstream file(fileName, std::ios::in);
	std::string text;
	if (file) {
		/* Go to end of file to determine size of file. */
		file.seekg(0, std::ios::end);
		text.resize(file.tellg());
		/* Go back to beginning and read file. */
		file.seekg(0, std::ios::beg);
		file.read(&text[0], text.size());
		file.close();
	}
	else {
		std::string errorMessage = std::string("File not found. ") + fileName;
		fprintf(stderr, errorMessage.c_str());
		throw std::runtime_error(errorMessage);
	}
	return text;
}
