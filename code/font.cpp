
namespace font {

const s32 ASCII_GLYPHS_COUNT  = ('~' - ' ');


Glyph_Info get_glyph(Font font, s32 code_point) {
	return(font.glyphs[code_point - ' ']);
}

Vec4i get_glyph_rect(Font font, s32 code_point) {
	return(font.rects[code_point - ' ']);
}

// @Todo: Make font.info be a pointer not a value type.
s32 get_glyph_kern_advance(Font font, s32 index1, s32 index2) {
	s32 r = stbtt_GetGlyphKernAdvance(font.info, index1, index2);
	return r;
}

Font load(M_Arena* arena, Str8 path, f32 font_size) {
	
	file_io::File file = file_io::load(path);
	if(!file_io::ok(&file)) Assert(!"Invalid file");
	
	stbtt_fontinfo* font_info = m_alloc_struct(arena, stbtt_fontinfo);
	s32 init_success = stbtt_InitFont(font_info, (u8*)file.data, 0);
	if(!init_success) Assert(!"Font init failed");
	
	f32 sf = stbtt_ScaleForPixelHeight(font_info, font_size);
	
	s32 ascent, descent, line_gap;
	stbtt_GetFontVMetrics(font_info, &ascent, &descent, &line_gap);
	s32 base_line = ascent*sf;
	
	const s32 glyph_count = ASCII_GLYPHS_COUNT;
	
	Glyph_Info* glyphs = m_alloc_array(arena, Glyph_Info, glyph_count);
	Vec4i*      rects  = m_alloc_array(arena, Vec4i, glyph_count);
	
	// Get glyph info.
	for(s32 c = ' '; c <= '~'; c += 1) {
		s32 index = stbtt_FindGlyphIndex(font_info, c);
		
		s32 advance, lsb;
		stbtt_GetGlyphHMetrics(font_info, index, &advance, &lsb);
		
		s32 bx0, bx1, by0, by1;
		stbtt_GetGlyphBitmapBox(font_info, index, sf, sf, &bx0, &by0, &bx1, &by1);
		s32 bbw = bx1 - bx0;
		s32 bbh = by1 - by0;
		
		s32 glyph_num = c - ' ';
		glyphs[glyph_num] = {c, index, by0, bbw, bbh, advance, lsb};
	}
	
	// Sorting glyphs by their widths, first come the smallest. This is sellection sort, I guess.
	// Note: Sorting by width seems to gives us the best result. I used to sort initial with
	// the height. I think I genuienly came up with a good algorithm for text packing on
	// accident.
	Glyph_Info* glyphs_sorted[glyph_count] = {};
	Loop(i, glyph_count) glyphs_sorted[i] = &glyphs[i];
	
	for(s32 outter = 0; outter < glyph_count; outter += 1) {
		s32 small_at = outter;
		s32 small    = glyphs_sorted[small_at]->width;
		for(s32 inner = outter + 1; inner < glyph_count; inner += 1) {
			s32 curr = glyphs_sorted[inner]->width;
			if(curr < small) {
				small    = curr;
				small_at = inner;
			}
		}
		
		Glyph_Info* temp = glyphs_sorted[outter];
		glyphs_sorted[outter] = glyphs_sorted[small_at];
		glyphs_sorted[small_at] = temp;
	}
	
	// Calculate pack coords for glyphs.
	s32 bb_x0, bb_x1, bb_y0, bb_y1;
	stbtt_GetFontBoundingBox(font_info, &bb_x0, &bb_y0, &bb_x1, &bb_y1);
	s32 bb_width  = Ceil((bb_x1 - bb_x0)*sf);
	s32 bb_height = Ceil((bb_y1 - bb_y0)*sf);
	
	s32 vert_padding = 2;
	s32 horz_padding = 2;
	
	s32 atlas_height = 4*(bb_height + 2*vert_padding);
	s32 atlas_width = 0;
	
	s32 pack_width = 0;
	s32 pack_height_left = atlas_height;
	
	Loop(i, glyph_count) {
		Glyph_Info* curr = glyphs_sorted[i];
		
		s32 padded_height = curr->height + 2*vert_padding;
		s32 padded_width  = curr->width + 2*horz_padding;
		
		if(pack_height_left - padded_height < 0) {
			atlas_width += pack_width;
			pack_height_left = atlas_height;
			
			pack_width = 0;
		}
		
		s32 x = atlas_width + horz_padding;
		s32 y = atlas_height - pack_height_left + vert_padding;
		s32 width = curr->width;
		s32 height = curr->height;
		
		s32 code_point = curr->code_point;
		
		rects[code_point - ' '] = {x, y, width, height};
		
		if(padded_width > pack_width) pack_width = padded_width;
		pack_height_left -= padded_height;
	}
	atlas_width += pack_width;
	
	// Packing mono
	M_Arena_Frame mem_frame = m_arena_start_frame(arena);
	
	u8* mono_data = m_arena_alloc(arena, atlas_width*atlas_height);
	
	Loop(i, glyph_count) {
		Glyph_Info* g = &glyphs[i];
		
		Vec4i rect = rects[g->code_point - ' '];
		
		u8* out = mono_data + rect.x + rect.y*atlas_width;
		stbtt_MakeGlyphBitmap(font_info, out, rect.width, rect.height, atlas_width, sf, sf, g->index);
	}
	
	u8* data = m_arena_alloc(arena, atlas_width*atlas_height*sizeof(u32));
	
	// Mono to RGBA format.
	u32* pixels   = (u32*)data;
	u8*  mono_ptr = (u8*)(mono_data + (atlas_height - 1)*atlas_width);
	
	Loop(y, atlas_height) {
		u8* mono_pixels = mono_ptr;
		Loop(x, atlas_width) {
			*pixels = 0xFFFFFF | ((u32)*mono_pixels << 24);
			pixels += 1;
			mono_pixels += 1;
		}
		mono_ptr -= atlas_width;
	}
	
	
	// Generate the texture.
	u32 id;
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	s32 lod = 0;
	s32 border_always_zero = 0;
	glTexImage2D(GL_TEXTURE_2D, lod, GL_RGBA, atlas_width, atlas_height, border_always_zero,
		GL_BGRA, GL_UNSIGNED_BYTE, data);
	
	m_arena_end_frame(arena, mem_frame);
	
	Font font = {};
	
	font.info         = font_info;
	font.base_size    = font_size;
	font.scale_factor = sf;
	font.base_line    = base_line;
	font.rects        = rects;
	font.glyphs       = glyphs;
	font.atlas        = {id, atlas_width, atlas_height, texture::Texture_Filter_Linear, texture::Texture_Format_RGBA};
	
	return font;
}

s32 text_width(Font font, Str8 text) {
	s32 width = 0;
	
	f32 width_f32 = 0.0f;
	Loop(i, text.size) {
		s32 code_point = text.data[i];
		Glyph_Info glyph = get_glyph(font, code_point);
		s32 advance = glyph.advance;
		
		b32 has_char_forward = i + 1 < text.size;
		if(has_char_forward) {
			s32 next_code_point   = text.data[i + 1];
			Glyph_Info next_glyph = get_glyph(font, next_code_point);
			
			s32 kern_advance = get_glyph_kern_advance(font, glyph.index, next_glyph.index);
			advance += kern_advance;
		}
		
		width_f32 += (f32)advance*font.scale_factor;
	}
	
	width = (s32)width_f32;
	return width;
}

} // namespace font