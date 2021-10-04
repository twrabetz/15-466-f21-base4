#include <ft2build.h>
#include FT_FREETYPE_H

#include <map>

#include <hb.h>
#include <hb-ft.h>

#include <iostream>
#include <glm/glm.hpp>

#include <vector>

#include "GL.hpp"

//Added stuff from https://github.com/harfbuzz/harfbuzz-tutorial/blob/master/hello-harfbuzz-freetype.c
//And https://www.freetype.org/freetype2/docs/tutorial/step1.html
//Used stuff from https://github.com/ChunanGang/TextBasedGame (which was kindly posted on Discord) for the OpenGL drawing component

struct GlyphTexture
{
	GLuint textureID;
	glm::ivec2 Size;
	glm::ivec2 Bearing;
};

struct FontID
{
	std::string name;
	int fontSize;

	FontID(std::string newName, int newFontSize)
	{
		name = newName;
		fontSize = newFontSize;
	}
};

class TextRenderer
{

	FT_Library library;
	FT_Error ft_error;

	FT_Face ft_face;

	hb_font_t* hb_font;

	unsigned int hb_buffer_len;
	hb_glyph_info_t* info;
	hb_glyph_position_t* pos;

	hb_buffer_t* hb_buffer = nullptr;

	std::vector<int> line_starts;
	std::vector<int> line_ends;

	GLuint VAO;
	GLuint VBO;

	int font_size;
	float xOffset;
	float yOffset;

	float x_space;
	float y_space;

	FontID fontID;

	bool centered;
	std::vector<float> center_offsets;

	bool overflow;
	TextRenderer* overflowRenderer = nullptr;

	glm::vec3 color;

public:

	static std::map<FontID, std::map<hb_codepoint_t, GlyphTexture>> renderedGlyphs;

	TextRenderer(std::string font_file, int font_size, float xOffset, float yOffset, float x_space, float y_space, bool centered, TextRenderer* overflow,
		bool isOverflow, glm::vec3 new_color);
	~TextRenderer();

	void draw();

	void set_text(std::string newText);

	void clear_text();

	void arrange_overflow_text(unsigned int hb_buffer_len, hb_glyph_info_t* info, hb_glyph_position_t* pos, std::vector<bool>& spaces,
		std::vector<bool>& returns, unsigned int i);

};