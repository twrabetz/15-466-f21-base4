#include "TextRenderer.hpp"

#include "TextRenderProgram.hpp"

#include <iostream>

std::map<FontID, std::map<hb_codepoint_t, GlyphTexture>> TextRenderer::renderedGlyphs = std::map<FontID, std::map<hb_codepoint_t, GlyphTexture>>();

bool operator<(const FontID& t1, const FontID& t2) {
	return (t1.name < t2.name) || (t1.name == t2.name && t1.fontSize < t2.fontSize);
}

TextRenderer::TextRenderer (std::string font_file, int new_font_size, float new_xOffset, float new_yOffset, float new_x_space, float new_y_space, bool new_centered,
	TextRenderer* new_overflow_renderer, bool isOverflow, glm::vec3 new_color)
	: fontID(font_file, new_font_size)
{
	x_space = new_x_space;
	y_space = new_y_space;
	xOffset = new_xOffset;
	yOffset = new_yOffset;
	centered = new_centered;
	font_size = new_font_size;
	overflowRenderer = new_overflow_renderer;
	overflow = isOverflow;
	color = new_color;

	if (ft_error = FT_Init_FreeType(&library))
	{
		std::cout << "Error with init freetype!" << std::endl;
		return;
	}

	if (ft_error = FT_New_Face(library, font_file.c_str(), 0, &ft_face))
	{
		std::cout << "Error with FT new face!" << ft_error << std::endl;
		return;
	}

	if (ft_error = FT_Set_Char_Size(ft_face, font_size * 64, font_size * 64, 0, 0))
	{
		std::cout << "Error with FT Set Char Size!" << ft_error << std::endl;
		return;
	}

	hb_font = hb_ft_font_create(ft_face, NULL);

	//This section of code is from https://github.com/ChunanGang/TextBasedGame
	// disable alignment since what we read from the face (font) is grey-scale
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// set up vao, vbo for the quad on which we render the glyph bitmap
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// each vertex has 4 floats, and we need 6 vertices for a quad
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	// set attribute in location 0, which is the in-vertex for vertex shader.
	glEnableVertexAttribArray(0);
	// tells vao how to read from buffer. (read 4 floats each time, which is one vertex)
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

TextRenderer::~TextRenderer()
{
	//This section of code is from https://github.com/ChunanGang/TextBasedGame, with adjustments
	FT_Done_Face(ft_face);
	FT_Done_FreeType(library);
	hb_font_destroy(hb_font);
	if( !overflow && hb_buffer != nullptr )
		hb_buffer_destroy(hb_buffer);
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);
}

void TextRenderer::set_text(std::string newText)
{

	if( hb_buffer != nullptr )
		hb_buffer_destroy(hb_buffer);

	line_starts.clear();
	line_ends.clear();
	if( centered )
		center_offsets.clear();

	/* Create hb-buffers and populate. */
	hb_buffer = hb_buffer_create();
	hb_buffer_add_utf8(hb_buffer, newText.c_str(), -1, 0, -1);
	hb_buffer_guess_segment_properties(hb_buffer);

	//Find the spaces in the buffer before shaping
	hb_buffer_len = hb_buffer_get_length(hb_buffer);
	info = hb_buffer_get_glyph_infos(hb_buffer, NULL);

	/* Shape it! */
	hb_shape(hb_font, hb_buffer, NULL, 0);

	std::vector<bool> spaces;
	std::vector<bool> returns;
	for (unsigned int i = 0; i < hb_buffer_len; i++)
	{
		spaces.push_back(newText[info[i].cluster] == ' ');
		returns.push_back(newText[info[i].cluster] == '\n');
	}
	

	/* Get glyph information and positions out of the buffer. */
	hb_buffer_len = hb_buffer_get_length(hb_buffer);
	info = hb_buffer_get_glyph_infos(hb_buffer, NULL);
	pos = hb_buffer_get_glyph_positions(hb_buffer, NULL);

	//Render the glyphs if needed

	if (renderedGlyphs.find(fontID) == renderedGlyphs.end())
	{
		renderedGlyphs[fontID] = std::map<hb_codepoint_t, GlyphTexture>();
	}

	for (unsigned int i = 0; i < hb_buffer_len; i++)
	{
		if (renderedGlyphs[fontID].find(info[i].codepoint) == renderedGlyphs[fontID].end())
		{
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

			//This section of code is from https://github.com/ChunanGang/TextBasedGame
			// generate buffer
			GLuint texture;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			// upload the bitmap to texure buffer
			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_RED,
				ft_face->glyph->bitmap.width,
				ft_face->glyph->bitmap.rows,
				0,
				GL_RED,
				GL_UNSIGNED_BYTE,
				ft_face->glyph->bitmap.buffer
			);
			// set some texture options
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			// store into the map
			GlyphTexture glyph = {
				texture,
				glm::ivec2(ft_face->glyph->bitmap.width, ft_face->glyph->bitmap.rows),
				glm::ivec2(ft_face->glyph->bitmap_left, ft_face->glyph->bitmap_top),
			};
			renderedGlyphs[fontID][info[i].codepoint] = glyph;
		}
	}

	//Split the buffer into lines
	{
		unsigned int i = 0;
		float yOffset = 0.0f;
		while (i < hb_buffer_len)
		{
			if (yOffset >= y_space)
			{
				if (overflowRenderer != nullptr)
					overflowRenderer->arrange_overflow_text(hb_buffer_len, info, pos, spaces, returns, i);
				break;
			}
			float offset = 0.0f;
			line_starts.push_back(i);
			unsigned int start = i;
			while (true)
			{
				unsigned int startOfWord = i;
				float startOffset = offset;
				//Advance by one word
				while (i < hb_buffer_len && !spaces[i] && !returns[i])
				{
					offset += pos[i].x_advance / 64.0f;
					i++;
				}
				//Move past the ending space, or go beyond the array
				i++;
				if (offset > x_space)
				{
					line_ends.push_back(startOfWord);
					if (centered)
						center_offsets.push_back(startOffset / 2);
					yOffset += font_size;
					i = startOfWord;
					break;
				}
				if (i >= hb_buffer_len)
				{
					line_ends.push_back(hb_buffer_len);
					yOffset += font_size;
					if (centered)
						center_offsets.push_back(offset / 2);
					break;
				}
				if (returns[i - 1])
				{
					line_ends.push_back(i - 1);
					if (centered)
						center_offsets.push_back(offset / 2);
					yOffset += font_size;
					break;
				}
				if (spaces[i - 1])
					offset += pos[i - 1].x_advance / 64.0f;
			}
		}
		if (i >= hb_buffer_len)
		{
			if (overflowRenderer != nullptr)
				overflowRenderer->clear_text();
		}
	}

}

void TextRenderer::arrange_overflow_text(unsigned int new_hb_buffer_len, hb_glyph_info_t* new_info, hb_glyph_position_t* new_pos, 
	std::vector<bool>& spaces, std::vector<bool>& returns, unsigned int i)
{
	hb_buffer_len = new_hb_buffer_len;
	info = new_info;
	pos = new_pos;

	line_starts.clear();
	line_ends.clear();

	//Split the buffer into lines
	{
		float yOffset = 0.0f;
		while (i < hb_buffer_len)
		{
			if (yOffset >= y_space)
			{
				if (overflowRenderer != nullptr)
					overflowRenderer->arrange_overflow_text(hb_buffer_len, info, pos, spaces, returns, i);
				break;
			}
			float offset = 0.0f;
			line_starts.push_back(i);
			unsigned int start = i;
			while (true)
			{
				unsigned int startOfWord = i;
				float startOffset = offset;
				//Advance by one word
				while (i < hb_buffer_len && !spaces[i] && !returns[i])
				{
					offset += pos[i].x_advance / 64.0f;
					i++;
				}
				//Move past the ending space, or go beyond the array
				i++;
				if (offset > x_space)
				{
					line_ends.push_back(startOfWord);
					if (centered)
						center_offsets.push_back(startOffset / 2);
					yOffset += font_size;
					i = startOfWord;
					break;
				}
				if (i >= hb_buffer_len)
				{
					line_ends.push_back(hb_buffer_len);
					yOffset += font_size;
					if (centered)
						center_offsets.push_back(offset / 2);
					break;
				}
				if (returns[i - 1])
				{
					line_ends.push_back(i - 1);
					if (centered)
						center_offsets.push_back(offset / 2);
					yOffset += font_size;
					break;
				}
				if( spaces[i-1] )
					offset += pos[i-1].x_advance / 64.0f;
			}
		}
		if (i >= hb_buffer_len)
		{
			if (overflowRenderer != nullptr)
				overflowRenderer->clear_text();
		}
	}
}

void TextRenderer::clear_text()
{
	line_starts.clear();
	line_ends.clear();

	if (overflowRenderer != nullptr)
		overflowRenderer->clear_text();
}

void TextRenderer::draw()
{
	if (overflowRenderer != nullptr)
		overflowRenderer->draw();

	//This section of code is from https://github.com/ChunanGang/TextBasedGame, with some adjustments
	// enable for text drawing
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// set the text rendering shaders
	glUseProgram(text_render_program->program);

	// pass in uniforms
	glUniform3f(glGetUniformLocation(text_render_program->program, "textColor"), color.x, color.y, color.z);
	// for the projection matrix we use orthognal
	glm::mat4 projection = glm::ortho(0.0f, 1920.0f, 0.0f, 1080.0f);
	glUniformMatrix4fv(glGetUniformLocation(text_render_program->program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	// rendering buffers setup
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(VAO);

	for (int k = 0; k < line_starts.size(); k++)
	{
		float x = xOffset;
		if (centered)
			x = 960.0f + xOffset - center_offsets[k];
		float y = 950.0f - yOffset - k * font_size;

		for (int i = line_starts[k]; i < line_ends[k]; i++)
		{
			// first get the hb shaping infos (offset & advance)
			float x_offset = pos[i].x_offset / 64.0f;
			float y_offset = pos[i].y_offset / 64.0f;
			float x_advance = pos[i].x_advance / 64.0f;
			float y_advance = pos[i].y_advance / 64.0f;

			// take out the glyph using the codepoint from hb buffer
			GlyphTexture glyph = renderedGlyphs[fontID][info[i].codepoint];
			// calculate actual position
			float xpos = x + (x_offset + glyph.Bearing.x);
			float ypos = y + (y_offset - (glyph.Size.y - glyph.Bearing.y));
			float w = (float)glyph.Size.x;
			float h = (float)glyph.Size.y;

			// update VBO for each glyph (6 vertices to draw a quad, which holds a glyph)
			// the info for each vector is (pos_x, pox_y, texture_coord_x, texture_coord_y)
			float vertices[6][4] = {
				{ xpos,     ypos + h,   0.0f, 0.0f },
				{ xpos,     ypos,       0.0f, 1.0f },
				{ xpos + w, ypos,       1.0f, 1.0f },

				{ xpos,     ypos + h,   0.0f, 0.0f },
				{ xpos + w, ypos,       1.0f, 1.0f },
				{ xpos + w, ypos + h,   1.0f, 0.0f }
			};
			// render glyph texture over quad
			glBindTexture(GL_TEXTURE_2D, glyph.textureID);
			// update content of VBO memory
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			// render quad
			glDrawArrays(GL_TRIANGLES, 0, 6);

			// advance to next graph, using the harfbuzz shaping info
			x += x_advance;
			y += y_advance;
		}
	}

	// unbind
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}