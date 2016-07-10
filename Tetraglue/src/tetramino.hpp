#pragma once

#include "extern.h"
#include <array>

enum Tetramino : uint8_t {
	I,
	O,
	L,
	J,
	S,
	Z,
	T
};

const size_t NUM_TETRAMINOE = 7;

// clockwise
typedef uint8_t Rotation;

const Rotation NUM_ROTATIONS = 4;

typedef std::array<size_t, 2> Coord; // value array type

struct Piece {
	uint8_t state;

public:
	Piece() {}
	Piece(Tetramino tetramino, Rotation rotation) : state((tetramino << 2) | rotation) {}

	Tetramino tetramino() const { return static_cast<Tetramino>(state >> 2); }
	Rotation rotation() const { return state & 3; }
	size_t width() const;
	size_t height() const;

	void rotate(Rotation delta) { Rotation total = (rotation() + delta) % NUM_ROTATIONS; state = (state &~3) | total; }
	// The most complex method here, tries to rotate the piece while not rotating it out of the game glass.
	Coord rotate(Rotation delta, size_t glass_width, Coord upleft);

	uint16_t mask() const;

	// This is a method that checks whether
private:
	Coord center_to_upleft(Coord, size_t) const;
	Coord upleft_to_center(Coord, size_t) const;
};
