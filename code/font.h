
#ifndef FONT_H
#define FONT_H

namespace font {
	namespace {
		struct Glyph_Info {
			s32 code_point;
			s32 index;
			s32 y0;
			s32 width, height;
			s32 advance, lsb;
		};
		
		struct Font {
			stbtt_fontinfo*  info;
			Vec4i*           rects;
			Glyph_Info*      glyphs;
			texture::Texture atlas;
			
			s32 base_size, base_line;
			f32 scale_factor;
			
		};
	}
	
	Glyph_Info get_glyph(Font font, s32 code_point);
	Vec4i      get_glyph_rect(Font font, s32 code_point);
	s32        get_glyph_kern_advance(Font font, s32 index1, s32 index2);
	
	Font load(M_Arena* arena, Str8 path, f32 font_size);
	
	s32 text_width(Font font, Str8 text);
}

#endif

