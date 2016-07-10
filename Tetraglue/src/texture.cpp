#include "texture.hpp"

static GLubyte FONT[]{
"    "
"    "
"    "
"    "
"    "
"    "
"    "
"    "

" ## "
"#  #"
"#  #"
"# ##"
"## #"
"#  #"
"#  #"
" ## "

"  # "
" ## "
"  # "
"  # "
"  # "
"  # "
"  # "
" ###"

" ## "
"#  #"
"   #"
"  # "
" #  "
"#   "
"#   "
"####"

" ## "
"#  #"
"   #"
"  # "
"   #"
"   #"
"#  #"
" ## "

"#   "
"#  #"
"#  #"
" ###"
"   #"
"   #"
"   #"
"   #"

"####"
"#   "
"#   "
"### "
"   #"
"   #"
"   #"
"### "

" ###"
"#   "
"#   "
"### "
"#  #"
"#  #"
"#  #"
" ## "

"####"
"   #"
"   #"
"  # "
" #  "
" #  "
" #  "
" #  "

" ## "
"#  #"
"#  #"
" ## "
"#  #"
"#  #"
"#  #"
" ## "


" ## "
"#  #"
"#  #"
" ###"
"   #"
"   #"
"#  #"
" ## "};

// I really hope this is properly allocated on a zeroed-out page. What a waste.
static const GLubyte ZEROS[std::max(2 * 8 * 11, 4 * 8 * 8)]{ 0 };

const GLuint TILES[4 * 8 * 8]{
// EMPTY
#define o 0xffffffff,
	o o o o o o o o
	o o o o o o o o
	o o o o o o o o
	o o o o o o o o
	o o o o o o o o
	o o o o o o o o
	o o o o o o o o
	o o o o o o o o
// CLEARED
#define o 0xf2d8d8ff,
#define O 0xd37c7cff,
#define M 0xba3e3eff,
#define U 0xe9bebeff,
#define C 0x822b2bff,
#define H 0x712626ff,
#define T 0xc34e4eff,
#define Z 0x4c1919ff,
#define W 0x541c1cff,
o O O O O O O M
U C C C C C C H
U C C C C C C H
U C C C C C C H
U C C C C C C H
U C C C C C C H
U C C C C C C H
T Z Z Z Z Z Z W
// FULL
#define o 0xffe7d7ff,
#define O 0xff9f5fff,
#define M 0xeb6431ff,
#define U 0xffc59fff,
#define C 0xff6600ff,
#define H 0xd52600ff,
#define T 0xf47952ff,
#define Z 0xe92900ff,
#define W 0xbe2200ff,
	o O O O O O O M
	U C C C C C C H
	U C C C C C C H
	U C C C C C C H
	U C C C C C C H
	U C C C C C C H
	U C C C C C C H
	T Z Z Z Z Z Z W
// PIECE
#define o 0xf3e1ffff,
#define O 0xae2fffff,
#define M 0xaf63f8ff,
#define U 0xd593ffff,
#define C 0xebccffff,
#define H 0x842cf0ff,
#define T 0xc66cffff,
#define Z 0x9c00ffff,
#define W 0x8f0fe1ff,
	o O O O O O O M
	U C C C C C C H
	U C C C C C C H
	U C C C C C C H
	U C C C C C C H
	U C C C C C C H
	U C C C C C C H
	T Z Z Z Z Z Z W
};

#undef o
#undef O
#undef M
#undef U
#undef C
#undef H
#undef T
#undef Z
#undef W

TileTexture::TileTexture(GLsizei set_width, GLsizei set_height, GLsizei set_depth, GLsizei map_width, GLsizei map_height) {
	// Making a map texture
	glGenTextures(1, &map_texture);
	glBindTexture(GL_TEXTURE_2D, map_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA8, map_width, map_height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, nullptr);
	// Texture parameters >_<
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
	// Sampling parameters (。・艸・)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Making a set texture
	glGenTextures(1, &set_texture);
	glBindTexture(GL_TEXTURE_3D, set_texture);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA8, set_width, set_height, set_depth, 0, GL_BGRA, GL_UNSIGNED_BYTE, nullptr);
	// Texture parameters
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAX_LEVEL, 0);

	GLint test_width;
	glGetTexLevelParameteriv(GL_TEXTURE_3D, 0, GL_TEXTURE_WIDTH, &test_width);
	if (test_width != set_width)
		abort();
	// Sampling parameters (2.0 has no sampler objects)
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	// generally the derived class will want to load it up immediately so don't unbind it

	// Setting up an uniform parameter
	tileParameters[0] = map_width;
	tileParameters[1] = map_height;
	tileParameters[3] = set_depth;
}

void TileTexture::bind() const {
	glCheck(glActiveTexture(GL_TEXTURE0 + SET_TARGET));
	glCheck(glBindTexture(GL_TEXTURE_3D, set_texture));
	glCheck(glActiveTexture(GL_TEXTURE0 + MAP_TARGET));
	glCheck(glBindTexture(GL_TEXTURE_2D, map_texture));
}

Font::Font() : TileTexture(8, 8, 11, Font::MAX_TEXT, 1) {
	if (FONT[0] != 0) {
		std::replace(FONT + 0, FONT + (4 * 8 * 11), (GLubyte)' ', (GLubyte)0);
		std::replace(FONT + 0, FONT + (4 * 8 * 11), (GLubyte)'#', (GLubyte)-1);
	}
	// clear the sides...
	glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, 2, 8, 11, GL_ALPHA, GL_UNSIGNED_BYTE, ZEROS);
	glTexSubImage3D(GL_TEXTURE_3D, 0, 6, 0, 0, 2, 8, 11, GL_ALPHA, GL_UNSIGNED_BYTE, ZEROS);
	// and load the middle.
	glTexSubImage3D(GL_TEXTURE_3D, 0, 2, 0, 0, 4, 8, 11, GL_ALPHA, GL_UNSIGNED_BYTE, FONT);
	glBindTexture(GL_TEXTURE_3D, 0);
}

void Font::render(unsigned int x) {
	x = std::min(x, MAX_NUMBER);
	int idx = MAX_TEXT - 1;
	do {
		text[idx--] = (x % 10) + 1;
		x /= 10;
	} while (x);
	while (idx >= 0) { text[idx--] = 0; }
	
	// Just push it to opengl right now, not like we have pixel buffer objects in 2.0 :(
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, MAX_TEXT, 1, GL_ALPHA, GL_UNSIGNED_BYTE, text);
}

LookingGlass::LookingGlass() : TileTexture(8, 8, 4, Glass::REAL_HEIGHT, Glass::WIDTH) {
	// pretty (ﾉ´ヮ´)ﾉ*:･ﾟ✧
	glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, 8, 8, 4, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, TILES);
	glBindTexture(GL_TEXTURE_3D, 0);
}

void LookingGlass::mirror(Glass const &glass) {
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, Glass::REAL_HEIGHT, Glass::WIDTH, GL_ALPHA, GL_UNSIGNED_BYTE, glass.data_pointer());
}

Premonition::Premonition() : TileTexture(8, 8, 2, 8, 8) {
	glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, 8, 8, 1, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, ZEROS);
	glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 1, 8, 8, 1, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, TILES + 8 * 8 * 3);
	glBindTexture(GL_TEXTURE_3D, 0);
}

void Premonition::predict(Piece const &piece) {
	std::uint16_t mask = piece.mask();
	GLubyte texdata[16];
	for (size_t i = 0; i < 16; ++i) {
		texdata[i] = (mask & (1 << i)) ? 1 : 0;
	}
	glBindTexture(GL_TEXTURE_2D, map_texture);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 4, 4, GL_ALPHA, GL_UNSIGNED_BYTE, texdata);
}
