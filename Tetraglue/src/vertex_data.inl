// This file describes vertex data for the main window.
// It is not a proper header file and is not meant to be included as such.

// The vertex data we need, interleaved.
struct Vertex {
	GLfloat vx, vy;
	GLfloat tx, ty;
};

static const int
WINDOW_WIDTH = 500,
WINDOW_HEIGHT = 700,
TILE_SIZE = 32,
GLASS_WIDTH = Glass::WIDTH * TILE_SIZE,
GLASS_HEIGHT = Glass::HEIGHT * TILE_SIZE,
NEXTPIECE_WIDTH = 4 * TILE_SIZE,
DIGIT_SIZE = 24,
TEXT_WIDTH = Font::MAX_TEXT * DIGIT_SIZE;

static const GLfloat
VBORDER = (WINDOW_HEIGHT - GLASS_HEIGHT) * 0.5f,
HBORDER = (WINDOW_WIDTH - GLASS_WIDTH - NEXTPIECE_WIDTH) / 3.f,
RIGHTPANE_CENTER = (VBORDER + GLASS_WIDTH + WINDOW_WIDTH) * 0.5f,
TEXT_TOP = (VBORDER + 4 * TILE_SIZE + VBORDER),
TEXT_LEFT = RIGHTPANE_CENTER - TEXT_WIDTH * 0.5f,
NEXTPIECE_VCENTER = VBORDER + 2 * TILE_SIZE;

// Constant coordinates.

// These are clockwise because the matrix is upside-down.
// Also, for the glass, the tile texture is transposed for implementation reasons.
static const Vertex glassCoords[4] = {
	{HBORDER, VBORDER, 0.0f, 0.0f},
	{HBORDER + GLASS_WIDTH, VBORDER, 0.0f, Glass::WIDTH},
	{HBORDER + GLASS_WIDTH, VBORDER + GLASS_HEIGHT, Glass::HEIGHT, Glass::WIDTH},
	{HBORDER, VBORDER + GLASS_HEIGHT, Glass::HEIGHT, 0.0f}
};

static const Vertex textCoords[4] = {
	{TEXT_LEFT, TEXT_TOP, 0.0f, 0.0f},
	{TEXT_LEFT + TEXT_WIDTH, TEXT_TOP, Font::MAX_TEXT, 0.0f},
	{TEXT_LEFT + TEXT_WIDTH, TEXT_TOP + DIGIT_SIZE, Font::MAX_TEXT, 1.0f},
	{TEXT_LEFT, TEXT_TOP + DIGIT_SIZE, 0.0f, 1.0f}
};

enum VertexBufferIndex : size_t {
	GLASS_VERTEX_BUFFER,
	NEXTPIECE_VERTEX_BUFFER,
	TEXT_VERTEX_BUFFER,
	NUM_VERTEX_BUFFERS
};

struct VertexBuffers {
	GLuint buffers[NUM_VERTEX_BUFFERS];

	VertexBuffers() {
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		glGenBuffers(NUM_VERTEX_BUFFERS, buffers);
		glBindBuffer(GL_ARRAY_BUFFER, buffers[GLASS_VERTEX_BUFFER]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glassCoords), glassCoords, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, buffers[NEXTPIECE_VERTEX_BUFFER]);
		glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, buffers[TEXT_VERTEX_BUFFER]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(textCoords), textCoords, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	VertexBuffers(VertexBuffers&) = delete;

	void update_nextpiece(Piece const &piece) {
		size_t width = piece.width(), height = piece.height();
		GLfloat halfwidth = (width * TILE_SIZE) * 0.5f, halfheight = (height * TILE_SIZE) * 0.5f;
		// this is as transposed as the main glass
		Vertex nextpiece_coords[4] = {
			{RIGHTPANE_CENTER - halfwidth, NEXTPIECE_VCENTER - halfheight, 0.0f, 0.0f},
			{RIGHTPANE_CENTER + halfwidth, NEXTPIECE_VCENTER - halfheight, 0.0f, width},
			{RIGHTPANE_CENTER + halfwidth, NEXTPIECE_VCENTER + halfheight, height, width},
			{RIGHTPANE_CENTER - halfwidth, NEXTPIECE_VCENTER + halfheight, height, 0.0f}
		};
		// invalidate previous data to not disrupt the GPU
		glBindBuffer(GL_ARRAY_BUFFER, buffers[NEXTPIECE_VERTEX_BUFFER]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(nextpiece_coords), nullptr, GL_DYNAMIC_DRAW);
		glBufferData(GL_ARRAY_BUFFER, sizeof(nextpiece_coords), nextpiece_coords, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void draw(VertexBufferIndex target) {
		glCheck(glBindBuffer(GL_ARRAY_BUFFER, buffers[target]));
		glVertexPointer(2, GL_FLOAT, sizeof(Vertex), reinterpret_cast<void*>(0));
		glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, tx)));
		glCheck(glDrawArrays(GL_TRIANGLE_FAN, 0, 4));
	}
};