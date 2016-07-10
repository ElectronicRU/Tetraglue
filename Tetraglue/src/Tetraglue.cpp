// Tetraglue.cpp: определяет точку входа для приложения.
//
#include "extern.h"
#include <string>
#include <stdexcept>
#include <iostream>
#include <SFML/System/Clock.hpp>

#include "glass.hpp"
#include "texture.hpp"

// Data files
#include "vertex_data.inl"
#include "tile_shader.inl"

class initialization_failed : public std::runtime_error {
public:
	initialization_failed(std::string const &what) : std::runtime_error(what) {}
};

void _postcall(const char *name, void *ptr, int num_args, ...) {
	GLenum error = glad_glGetError();
	if (error == GL_INVALID_VALUE) {
		std::cerr << "error in " << name << std::endl;
		abort();
	}
}

// This code uses a lot of legacy stuff. For example, it doesn't use samplers,
// so sampling parameters need to be set manually for each texture;
// instead of using generic vertex attributes and uniform matrices, it uses
// bits and parts of fixed pipeline; and finally (the most travesty)
// it actually uses GL_ALPHA8 as an image format, which is an offense to shoot at sight.
int main(void) {
	// Initialize all things necessary.
	sf::ContextSettings settings(0, 0, 0, 2, 0);

	sf::Window window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Tetraglue", sf::Style::Titlebar | sf::Style::Close, settings);

	if (!gladLoadGL()) {
		std::cerr << "Failed to load GL extensions." << std::endl;
		system("pause");
		return -1;
	}
#ifdef GLAD_DEBUG
	glad_set_post_callback(_postcall);
#endif

	window.setVerticalSyncEnabled(true);

	// Set the basic stuff that is not ever going to change.
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_TEXTURE_3D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(0.5f, 0.5f, 0.5f, 0.5f);
	glOrtho(0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, -1, 1);

	// Create helper objects.
	Font numeric_font{};
	VertexBuffers vbufs{};
	TileShader tile_shader{};
	LookingGlass modelview{};
	Premonition user_experience{};

	bool window_open = true, running = true;
	bool speedup = false;
	bool redraw = true, rescore = true, update_next_piece = true;

	// Timekeeping.
	sf::Clock clock;
	sf::Time time, interval = sf::seconds(1.0f);
	// Simulation
	Glass glass;
	// Score
	unsigned int score = 0, combo = 0;
	while (window_open) {
		sf::Event event;
		while (window.pollEvent(event)) {
			switch (event.type) {
			case sf::Event::Closed:
				window_open = false;
				break;
			case sf::Event::KeyPressed:
				switch (event.key.code) {
				case sf::Keyboard::Left:
				case sf::Keyboard::Numpad4:
				case sf::Keyboard::A:
					glass.move(-1);
					redraw = true;
					break;
				case sf::Keyboard::Right:
				case sf::Keyboard::Numpad6:
				case sf::Keyboard::D:
					glass.move(+1);
					redraw = true;
					break;
				case sf::Keyboard::Numpad7:
				case sf::Keyboard::Numpad1:
				case sf::Keyboard::Q:
					glass.rotate(-1);
					redraw = true;
					break;
				case sf::Keyboard::Numpad9:
				case sf::Keyboard::Numpad3:
				case sf::Keyboard::E:
					glass.rotate(1);
					redraw = true;
					break;
				case sf::Keyboard::Down:
				case sf::Keyboard::Numpad2:
				case sf::Keyboard::S:
					speedup = true;
					break;
				}
				break;
			}
		}
		glCheck(glClear(GL_COLOR_BUFFER_BIT));
		time += clock.restart();
		if (running && (time > interval||speedup)) {
			if (speedup) {
				speedup = false;
			} else {
				time -= interval;
				interval *= 0.998f;
			}
			Glass::SimResult result = glass.simulation_step();
			switch (result) {
			case Glass::STOP:
				running = false;
				break;
			case Glass::AWARD_POINTS:
				score += (combo += 100);
				//numeric_font.render(score);
				if (score >= Font::MAX_NUMBER)
					running = false;
				break;
			case Glass::CONTINUE:
				combo = 0;
				break;
			case Glass::NEXT_PIECE:
				combo = 0;
				rescore = true;
				break;
			}
			redraw = true;
		}

		tile_shader.use(numeric_font);
		if (rescore) {
			rescore = false;
			numeric_font.render(score);
		}
		vbufs.draw(TEXT_VERTEX_BUFFER);

		tile_shader.use(modelview);
		if (redraw) {
			redraw = false;
			modelview.mirror(glass);
		}
		vbufs.draw(GLASS_VERTEX_BUFFER);
		tile_shader.use(user_experience);
		if (update_next_piece) {
			Piece const p = glass.get_next_piece();
			user_experience.predict(p);
			vbufs.update_nextpiece(p);
		}
		vbufs.draw(NEXTPIECE_VERTEX_BUFFER);
		window.display();
		glCheck(0);
	}
	while(true)

	return 0;
}