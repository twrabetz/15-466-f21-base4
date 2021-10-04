#include <stdlib.h>
#include <stdio.h>

#include <map>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <hb.h>
#include <hb-ft.h>

#include <iostream>

#include "GL.hpp"

#define FONT_SIZE 36
#define MARGIN (FONT_SIZE * .5)

//This file exists to check that programs that use freetype / harfbuzz link properly in this base code.
//You probably shouldn't be looking here to learn to use either library.

//Added stuff from https://github.com/harfbuzz/harfbuzz-tutorial/blob/master/hello-harfbuzz-freetype.c
//And https://www.freetype.org/freetype2/docs/tutorial/step1.html
int main(int argc, char **argv) {

	const char* fontfile;
	const char* text;

	if (argc < 3)
	{
		fprintf(stderr, "usage: freetype-test font-file.ttf text\n");
		exit(1);
	}

	fontfile = argv[1];
	text = argv[2];

	FT_Library library;

	FT_Error ft_error;
	if (ft_error = FT_Init_FreeType(&library))
	{
		std::cout << "Error with init freetype!" << std::endl;
		return -1;
	}

	FT_Face ft_face;

	if (ft_error = FT_New_Face(library, fontfile, 0, &ft_face))
	{
		std::cout << "Error with FT new face!" << std::endl;
		return -1;
	}

	if (ft_error = FT_Set_Char_Size(ft_face, FONT_SIZE * 64, FONT_SIZE * 64, 0, 0))
	{
		std::cout << "Error with FT Set Char Size!" << std::endl;
		return -1;
	}

	hb_font_t* hb_font = hb_ft_font_create(ft_face, NULL);

	/* Create hb-buffer and populate. */
	hb_buffer_t* hb_buffer;
	hb_buffer = hb_buffer_create();
	hb_buffer_add_utf8(hb_buffer, text, -1, 0, -1);
	hb_buffer_guess_segment_properties(hb_buffer);

	/* Shape it! */
	hb_shape(hb_font, hb_buffer, NULL, 0);

	/* Get glyph information and positions out of the buffer. */
	unsigned int len = hb_buffer_get_length(hb_buffer);
	hb_glyph_info_t* info = hb_buffer_get_glyph_infos(hb_buffer, NULL);
	hb_glyph_position_t* pos = hb_buffer_get_glyph_positions(hb_buffer, NULL);

	printf("Raw buffer contents:\n");
	for (unsigned int i = 0; i < len; i++)
	{
		hb_codepoint_t gid = info[i].codepoint;
		unsigned int cluster = info[i].cluster;
		double x_advance = pos[i].x_advance / 64.;
		double y_advance = pos[i].y_advance / 64.;
		double x_offset = pos[i].x_offset / 64.;
		double y_offset = pos[i].y_offset / 64.;

		char glyphname[32];
		hb_font_get_glyph_name(hb_font, gid, glyphname, sizeof(glyphname));

		printf("glyph='%s'	cluster=%d	advance=(%g,%g)	offset=(%g,%g)\n",
			glyphname, cluster, x_advance, y_advance, x_offset, y_offset);
	}

	printf("Converted to absolute positions:\n");
	/* And converted to absolute positions. */
	{
		double current_x = 0;
		double current_y = 0;
		for (unsigned int i = 0; i < len; i++)
		{
			hb_codepoint_t gid = info[i].codepoint;
			printf("Codepoint: %u", gid);
			unsigned int cluster = info[i].cluster;
			double x_position = current_x + pos[i].x_offset / 64.;
			double y_position = current_y + pos[i].y_offset / 64.;


			char glyphname[32];
			bool found = hb_font_get_glyph_name(hb_font, gid, glyphname, sizeof(glyphname));
			if (!found)
				printf("Glyph not found!\n");

			printf("glyph='%s'	cluster=%d	position=(%g,%g)\n",
				glyphname, cluster, x_position, y_position);

			current_x += pos[i].x_advance / 64.;
			current_y += pos[i].y_advance / 64.;
		}
	}

	//Render the glyphs
	std::map<hb_codepoint_t, FT_GlyphSlot> renderedGlyphs;

	for (unsigned int i = 0; i < len; i++)
	{
		if (renderedGlyphs.find(info[i].codepoint) == renderedGlyphs.end())
		{
			std::cout << "Rendering glyph " << info[i].codepoint << std::endl;
			if (ft_error = FT_Load_Glyph(ft_face, info[i].codepoint, FT_LOAD_DEFAULT))
			{
				std::cout << "Error in FT Load Glyph!" << std::endl;
			}
			if (ft_face->glyph->format != FT_GLYPH_FORMAT_BITMAP)
			{
				if (ft_error = FT_Render_Glyph(ft_face->glyph, FT_RENDER_MODE_NORMAL))
				{
					std::cout << "Error in FT Render Glyph!" << std::endl;
				}
			}
			renderedGlyphs[info[i].codepoint] = ft_face->glyph;
		}
	}

	hb_buffer_destroy(hb_buffer);
}
