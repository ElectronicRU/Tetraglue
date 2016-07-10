// This file describes tetramino tiles.
#include "tetramino.hpp"
#include <algorithm>

// Tetraminoe description.
// Each tetramino is a 16-bit mask that is interpreted as its squares. 4 consecutive bits describe a column.

static std::uint16_t TETRAMINO_ROTATIONS[NUM_TETRAMINOE * NUM_ROTATIONS] = {
	0xf,    0x1111, 0xf,    0x1111,
	0x33,   0x33,   0x33,   0x33,
	0x47,   0x113,  0x71,   0x322,
	0x74,   0x223,  0x17,   0x311,
	0x132,  0x63,   0x132,  0x63,
	0x231,  0x36,   0x231,  0x36,
	0x131,  0x72,   0x232,  0x27,
};

size_t Piece::width() const {
	std::uint16_t this_mask = mask();
	return (this_mask >= 0x1) + (this_mask >= 0x10) + (this_mask >= 0x100) + (this_mask >= 0x1000);
}

size_t Piece::height() const {
	std::uint16_t this_mask = mask();
	this_mask |= this_mask >> 8;
	this_mask |= this_mask >> 4;
	this_mask &= 0xf;
	return ((this_mask & 0x1) != 0) + ((this_mask & 0x2) != 0) + ((this_mask & 0x4) != 0) + ((this_mask & 0x8) != 0);
}

Coord Piece::center_to_upleft(Coord center, size_t width) const {
	// can't hold negative values
	return Coord{ std::max(center[0], width / 2) - width / 2, center[1] };
}

Coord Piece::upleft_to_center(Coord upleft, size_t width) const {
	return Coord{ upleft[0] + width / 2, upleft[1] };
}

Coord Piece::rotate(Rotation delta, size_t glass_width, Coord upleft) {
	size_t this_width = width();
	Coord center = upleft_to_center(upleft, this_width);
	rotate(delta);
	this_width = width();
	upleft = center_to_upleft(center, this_width);
	upleft[0] = std::min(upleft[0], glass_width - this_width);
	return upleft;
}

std::uint16_t Piece::mask() const {
	return TETRAMINO_ROTATIONS[state];
}