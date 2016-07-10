// The textures that use our AMAZING UNRIVALED shader come in pairs.
// Those are these pairs.
// Note that the update functions here (named differently because they're VERY different)
// will not bind or unbind the texture, because bind() already leaves the map texture bound.
#pragma once
#include "extern.h"
#include "glass.hpp"
#include "tetramino.hpp"

class TileTexture {
protected:
	GLuint map_texture, set_texture;
	TileTexture(GLsizei set_width,
		GLsizei set_height,
		GLsizei set_depth,
		GLsizei map_width,
		GLsizei map_height);
public:
	static const GLuint MAP_TARGET = 0, SET_TARGET = 1;
	void bind() const;
	GLfloat tileParameters[4];
};

class Font : public TileTexture {
public:
	static const size_t MAX_TEXT = 6;
	static const unsigned int MAX_NUMBER = 999999;
private:
	GLubyte text[MAX_TEXT];
public:
	Font();
	void render(unsigned int);
};

class LookingGlass : public TileTexture {
public:
	LookingGlass();
	void mirror(Glass const&);
};

class Premonition : public TileTexture {
public:
	Premonition();
	void predict(Piece const&);
};