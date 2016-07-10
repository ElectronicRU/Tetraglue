#pragma once

#include "extern.h"
#include <random>
#include "tetramino.hpp"

typedef std::uint8_t ubyte;


class Glass {
public:
	static const std::size_t WIDTH = 10, HEIGHT = 20;
	static const std::size_t REAL_HEIGHT = ((HEIGHT + 4) >> 2) << 2; // align to 4 and add at least 1
private:
	enum State {
		IDLE,
		FALLING,
		CLEARING
	};
	State state;
	Coord piece_coord;
	Piece current_piece;
	Piece next_piece;

	std::minstd_rand random_engine;
	static std::uniform_int_distribution<std::uint16_t> tetramino_rnd;
	static std::uniform_int_distribution<std::uint16_t> rotation_rnd;

	// The order is important here. CLEARED = EMPTY + 1, and PIECE = FULL + 1
	enum CellState : ubyte {
		EMPTY,
		CLEARED,
		FULL,
		PIECE
	};
	ubyte data[WIDTH][REAL_HEIGHT];
	ubyte helper[HEIGHT];

	void _generate_next_piece();
	bool _generate_piece_coord();
	bool _begin_falling();
	bool _rewrite_idle();
	bool _rewrite_falling();
	bool _rewrite_clearing();
public:
	Glass();

	enum SimResult {
		CONTINUE,
		AWARD_POINTS,
		NEXT_PIECE,
		STOP
	};
	SimResult simulation_step();
	void reset();
	void rotate(Rotation delta);
	void move(int delta);
	Piece get_next_piece() {
		return next_piece;
	};
	// This is a data pointer suitable for dumping into GPU, hopefully.
	void const *data_pointer() const {
		return reinterpret_cast<void const *>(data);
	}
};
