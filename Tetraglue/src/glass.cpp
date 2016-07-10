// The glass, into which the objects shall fall.

#include "glass.hpp"

// static
std::uniform_int_distribution<std::uint16_t> Glass::tetramino_rnd(0, NUM_TETRAMINOE - 1);
std::uniform_int_distribution<std::uint16_t> Glass::rotation_rnd(0, NUM_ROTATIONS - 1);

// Initial fill: make the guards at the bottom of the glass, seed the random engine.
Glass::Glass() {
	std::random_device rnd;
	random_engine.seed(rnd());
	reset();
}

void Glass::reset() {
	state = IDLE;
	for (std::size_t column = 0; column < WIDTH; ++column) {
		std::fill(data[column] + 0, data[column] + HEIGHT, EMPTY);
		std::fill(data[column] + HEIGHT, data[column] + REAL_HEIGHT, FULL);
	}
	if (_DEBUG)
		random_engine.seed(1U);
	_generate_next_piece();
}

void Glass::_generate_next_piece() {
	Tetramino type = static_cast<Tetramino>(tetramino_rnd(random_engine));
	Rotation rot = static_cast<Rotation>(rotation_rnd(random_engine));
	next_piece = Piece(type, rot);
}

// if this returns true, no piece could be generated
bool Glass::_generate_piece_coord() {
	std::uniform_int_distribution<size_t> column_rnd(0, WIDTH - current_piece.width());
	// have some leeway against really unlucky generations
	for (size_t num_try = 0; num_try < 3; ++num_try) {
		size_t column = column_rnd(random_engine);
		piece_coord = Coord{ column, 0 };
		if (!_rewrite_falling()) return false;
	}
	return true;
}

// Begin falling a new piece. Return true if successful.
bool Glass::_begin_falling() {
	// Generate a new piece.
	current_piece = next_piece;
	_generate_next_piece();
	// Randomly choose a coordinate for it.
	if (_generate_piece_coord()) {
		// Sad, sad days. Nowhere to fit the new piece, or we don't want to try harder.
		return false;
	}
	// _generate_piece_coord already did a rewrite for us, our job here is done
	state = FALLING;
	return true;
}

// Rewrite the data array according to the idle state, in at most two linear passes.
// Return whether there is stuff to clear.
bool Glass::_rewrite_idle() {
	std::fill(helper + 0, helper + HEIGHT, FULL);
	for (int8_t column = 0; column < WIDTH; ++column) {
		for (int8_t row = 0; row < HEIGHT; ++row) {
			data[column][row] &= ~1; // PIECE to FULL
			helper[row] = std::min(helper[row], data[column][row]);
		}
	}
	// check if there's any rows FULL in the whole array
	if (std::find(helper + 0, helper + HEIGHT, FULL) == helper + HEIGHT) {
		return false;
	}
	for (int8_t column = 0; column < WIDTH; ++column) {
		for (int8_t row = 0; row < HEIGHT; ++row) {
			if (helper[row]) {
				data[column][row] = CLEARED;
			}
		}
	}
	return true;
}

// Rewrite the data array according to the falling state, in two linear passes.
// Return whether the collision occured.
bool Glass::_rewrite_falling() {
	const size_t start_column = piece_coord[0], start_row = piece_coord[1];
	// First pass.
	std::replace(data[0] + 0, data[WIDTH - 1] + REAL_HEIGHT, PIECE, EMPTY);
	// Second pass.
	auto mask = current_piece.mask();
	bool COLLIDES = false;
	// Mask goes by 1 down each row, and by 4 down each column thus. When the mask is empty, we are done.
	for (size_t column = start_column; mask != 0; ++column) {
		std::replace(data[column] + 0, data[column] + start_row, PIECE, EMPTY);
		for (size_t row = start_row; row < start_row + 4; ++row, mask >>= 1) {
			if (mask & 1) {
				if (data[column][row]) {
					COLLIDES = true;
				}
				else {
					data[column][row] = PIECE;
				}
			}
		}
	}
	return COLLIDES;
}

// Rewrite the data array according to the clearing state, in less than two linear passes.
// Returns whether it did anything.
bool Glass::_rewrite_clearing() {
	// Find the index of CLEARED.
	size_t first_cleared = std::find(data[0] + 0, data[0] + HEIGHT, CLEARED) - data[0];
	if (first_cleared == HEIGHT)
		return false;
	for (int8_t column = 0; column < WIDTH; ++column) {
		ubyte carry = EMPTY;
		for (size_t row = 0; row <= first_cleared; ++row) {
			std::swap(carry, data[column][row]);
		}
	}
	return true;
}

// A simulation step. Returns true if everything is okay, or false if the game is, ultimately, over.
Glass::SimResult Glass::simulation_step() {
	switch (state) {
	case IDLE: {
		bool to_clear = _rewrite_idle();
		if (to_clear) {
			// There's not much work to do, everything has been done in _rewrite_idle.
			state = CLEARING;
			return CONTINUE;
		} else {
			if (_begin_falling()) {
				return NEXT_PIECE;
			} else {
				return STOP;
			}
		}
	};
	case FALLING: {
		// Try to move a piece down.
		++piece_coord[1];
		if (_rewrite_falling()) {
			// A collision occured, put stuff back where it was.
			--piece_coord[1];
			_rewrite_falling();
			state = IDLE;
			return CONTINUE;
		} else {
			// No collision! Awesome!
			return CONTINUE;
		}
	};
	case CLEARING: {
		if (_rewrite_clearing()) {
			return AWARD_POINTS;
		} else {
			state = IDLE;
			return CONTINUE;
		}
	};
	}
}

void Glass::rotate(Rotation delta) {
	if (state != FALLING)
		return;
	Coord old_coord = piece_coord;
	piece_coord = current_piece.rotate(delta, WIDTH, old_coord);
	if (_rewrite_falling()) {
		// Collision occured, put it back
		current_piece.rotate(-delta);
		piece_coord = old_coord;
		_rewrite_falling();
	}
}

void Glass::move(int delta) {
	if (state != FALLING)
		return;
	if (delta < 0 && piece_coord[0] < -delta)
		return;
	if (delta > 0 && piece_coord[0] + delta > WIDTH - current_piece.width())
		return;
	piece_coord[0] += delta;
	if (_rewrite_falling()) {
		// Collision occured, put it back
		piece_coord[0] -= delta;
		_rewrite_falling();
	}
}