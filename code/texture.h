

#ifndef TEXTURE_H
#define TEXTURE_H

namespace texture {
	namespace {
		enum Texture_Filter {
			Texture_Filter_Linear,
			Texture_Filter_Nearest,
		};
		
		
		enum Texture_Format {
			Texture_Format_RGBA,
		};
		
		struct Texture {
			u32 id;
			s32 width, height;
			Texture_Filter filter;
			Texture_Format format;
		};
	}
	
	Texture load(Str8 path);
	void unload(Texture texture);
	
	Vec4 sub(Texture texture, s32 x, s32 y, s32 width, s32 height);
	
	Texture white();
	Texture black();
	
}

#endif
