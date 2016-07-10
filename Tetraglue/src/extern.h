#pragma once
// External dependencies.

// C++ STL
#include <cstdint>
#include <algorithm>

using std::uint8_t;
using std::int8_t;
using std::size_t;

// OpenGL
#include <glad/glad.h>
#include <SFML/Window.hpp>
#include <SFML/Graphics/Shader.hpp>
#undef min // stupid windows.h
#undef max

#ifdef _DEBUG
#include <iostream>
#include <cstdlib>
// small utility
inline void glCheckError(const char *file, unsigned int line) {
	GLenum error = glGetError();
	const char *message;
	switch (error) {
	case GL_NO_ERROR:
		return;
	case GL_INVALID_ENUM:
		message = "Invalid enum"; break;
	case GL_INVALID_VALUE:
		message = "Value out of bounds"; break;
	case GL_INVALID_OPERATION:
		message = "Invalid operation"; break;
	default:
		message = "wow, weird.";
	}
	std::cerr << file << ":" << line << ": " << message << std::endl;
	std::exit(-1);
}

#define glCheck(expr) (glCheckError(__FILE__, __LINE__), expr)
#else
#define glCheck(expr) (expr)
#endif
