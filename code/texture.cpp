

namespace texture {

Texture load(Str8 path) {
	Texture result = {};
	
	s32 width, height, channels;
	s32 desired_channels = 4;
	
	stbi_set_flip_vertically_on_load(true);
	
	char path_cstr[512];
	str8_copy_to_cstr(path, path_cstr, 512);
	
	u8 *image_data = stbi_load(path_cstr, &width, &height, &channels, desired_channels);
	
	if(image_data) {
		u32 id;
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		
		s32 lod = 0;
		s32 border_always_zero = 0;
		glTexImage2D(GL_TEXTURE_2D, lod, GL_RGBA, width, height, border_always_zero,
			GL_RGBA, GL_UNSIGNED_BYTE, image_data);
		
		glBindTexture(GL_TEXTURE_2D, 0);
		
		stbi_image_free(image_data);
		
		result = {id, width, height, Texture_Filter_Linear, Texture_Format_RGBA};
	}else {
		printf("Can't create image data for texture!!! File might not exist!!!\n");
	}
	
	return result;
}

void unload(Texture texture) {
	glDeleteTextures(1, &texture.id);
}

Vec4 sub(Texture texture, s32 x, s32 y, s32 width, s32 height) {
	Vec4 sub = {};
	
	sub.top    = 1.0f - (f32)y/(f32)texture.height;
	sub.left   = (f32)x/(f32)texture.width;
	sub.bottom = sub.top - (f32)height/(f32)texture.height;
	sub.right  = sub.left + (f32)width/(f32)texture.width;
	return sub;
}


internal Texture the_white_texture, the_black_texture;
internal b32 got_the_white_texture = false, got_the_black_texture = false;

Texture white() {
	if(!got_the_white_texture) {
		u32 id;
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		
		u32 single_pixel_data = 0xFFFFFFFF;
		s32 lod = 0;
		s32 border_always_zero = 0;
		glTexImage2D(GL_TEXTURE_2D, lod, GL_RGBA, 1, 1, border_always_zero,
			GL_BGRA, GL_UNSIGNED_BYTE, &single_pixel_data);
		
		the_white_texture = {id, 1, 1, Texture_Filter_Linear, Texture_Format_RGBA};
		got_the_white_texture = true;
	}
	
	return the_white_texture;
}

Texture black() {
	if(!got_the_black_texture) {
		u32 id;
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		
		u32 single_pixel_data = 0xFF000000;
		s32 lod = 0;
		s32 border_always_zero = 0;
		glTexImage2D(GL_TEXTURE_2D, lod, GL_RGBA, 1, 1, border_always_zero,
			GL_BGRA, GL_UNSIGNED_BYTE, &single_pixel_data);
		
		the_black_texture = {id, 1, 1, Texture_Filter_Linear, Texture_Format_RGBA};
		got_the_black_texture = true;
	}
	
	return the_black_texture;
}

}
