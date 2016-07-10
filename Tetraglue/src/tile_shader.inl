// This file describes the code for tile shader, which is used throughout the application
// (in fact, for everything, including text output).
// It uses two samplers: one 1-component texture, which describes a 2d array indexing into
// the tileset, and the other 4-component, a proper tileset.
// (The shader chooses to be ignorant of tileset alpha, so we can use it with or without alpha
// test).
// A more proper OpenGL version would've made passing tileParameters uniform unnecessary,
// but alas, we are once again working with 2.0 for some godforsaken reason.

struct TileShader : sf::Shader {
	static const char *vertex_shader, *fragment_shader;
	GLint tile_param_loc;
	TileShader() : sf::Shader() {
		loadFromMemory(vertex_shader, fragment_shader);
		// Painful, oh painful parameter binding. Still worth using this class for error checking!
		sf::Shader::bind(this);
		GLuint program = getNativeHandle();
		GLint tile_map_loc = glCheck(glGetUniformLocation(program, "tileMap"));
		GLint tile_set_loc = glCheck(glGetUniformLocation(program, "tileSet"));
		tile_param_loc = glCheck(glGetUniformLocation(program, "tileParameters"));
		glCheck(glUniform1i(tile_map_loc, TileTexture::MAP_TARGET));
		glCheck(glUniform1i(tile_set_loc, TileTexture::SET_TARGET));
		sf::Shader::bind(nullptr);
	}

	void use(TileTexture const &texture) {
		texture.bind();
		sf::Shader::bind(this);
		glCheck(glUniform4fv(tile_param_loc, 1, texture.tileParameters));
	}
};

const char *TileShader::vertex_shader =
"#version 110\n"
"void main() {\n"
"    gl_Position = ftransform();\n"
"    gl_TexCoord[0] = gl_MultiTexCoord0;\n"
"}\n";

const char *TileShader::fragment_shader =
"#version 110\n"
"uniform sampler2D tileMap;\n"
"uniform sampler3D tileSet;\n"
"uniform vec4 tileParameters;\n" // tileParameters.st is tile map size, tileParameters.q is the size of the tile set.
"void main() {\n"
"	 vec2 texCoord = gl_TexCoord[0].xy;\n"
"    vec2 subTileCoord = fract(texCoord);\n"
"    float tileData = texture2D(tileMap, (floor(texCoord) + vec2(0.5, 0.5)) / tileParameters.st).a;\n"
// yes, OpenGL 2.0 uses ALPHA instead of red for monochannel. WHATEVER.
"    float tileNumber = floor(tileData * 256.0 + 0.1);\n" // OpenGL 2.0 doesn't support integer samplers. THE PAIN.
"    vec3 tileCoord = vec3(subTileCoord, (tileNumber + 0.5) / tileParameters.q);\n"
// the absence of texelFetch is creating jobs for all those 0.5s
#if 0
"	 gl_FragColor = vec4(subTileCoord, tileData * 25.60 / tileParameters.q, 1.0);\n"
#else
"    gl_FragColor = texture3D(tileSet, tileCoord);\n"
#endif
"}\n";
