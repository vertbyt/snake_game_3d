
namespace draw {

void text_line(font::Font font, Str8 text, Vec2 pos, u32 color) {
	f32 x = pos.x;
	f32 y = pos.y;
	
	draw_2d_immediate::texture(font.atlas);
	
	Loop(i, text.size) {
		s32 code_point = text.data[i];
		
		font::Glyph_Info glyph = font::get_glyph(font, code_point);
		Vec4i            rect  = font::get_glyph_rect(font, code_point);
		
		if(code_point != ' ') {
			f32 xx = x + (f32)glyph.lsb*font.scale_factor;
			f32 yy = y + glyph.y0 + font.base_line;
			
			Vec4 sub = texture::sub(font.atlas, rect.x, rect.y, rect.width, rect.height);
			draw_2d_immediate::quad({xx, yy, 1.0f}, vec2(rect.width, rect.height), sub, color);
		}
		
		s32 advance = glyph.advance;
		
		b32 has_char_forward = i + 1 < text.size;
		if(has_char_forward) {
			s32 next_code_point   = text.data[i + 1];
			font::Glyph_Info next_glyph = font::get_glyph(font, next_code_point);
			
			s32 kern_advance = font::get_glyph_kern_advance(font, glyph.index, next_glyph.index);
			advance += kern_advance;
		}
		
		x += (f32)advance*font.scale_factor;
	}
}

void world_grid(f32 y, s32 extents) {
	
	draw_3d_immediate::begin();
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	draw_3d_immediate::material({texture::white(), texture::black(), 1.0f});
	
	Loop(x, extents) {
		Loop(z, extents) {
			Vec3 p = {(f32)x, 0.0f, (f32)z};
			Vec3 tl = {-0.5f, y, 0.5f};
			Vec3 bl = {-0.5f, y, -0.5f};
			Vec3 br = {0.5f,  y, -0.5f};
			Vec3 tr = {0.5f,  y, 0.5f};
			
			Vec3 n = {0, 1, 0};
			draw_3d_immediate::vertex(p + tl, n, {}, 0xFFFFFF);
			draw_3d_immediate::vertex(p + bl, n, {}, 0xFFFFFF);
			draw_3d_immediate::vertex(p + tr, n, {}, 0xFFFFFF);
			
			draw_3d_immediate::vertex(p + tr, n, {}, 0xFFFFFF);
			draw_3d_immediate::vertex(p + bl, n, {}, 0xFFFFFF);
			draw_3d_immediate::vertex(p + br, n, {}, 0xFFFFFF);
		}
	}
	
	draw_3d_immediate::end();
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

internal void snake_draw(entities::Snake* snake) {
	
	Vec4 head_color = {1.0f, 0.4f, 0.4f, 1.0f};
	Vec4 body_color = {1.0f, 0.25f, 0.25f, 1.0f};
	
	switch(snake->color) {
		case entities::Snake_Color_None: {
			head_color = {0.4f, 0.4f,  0.4f,  1.0f};
			body_color = {0.25f, 0.25f, 0.25f, 1.0f};
		}break;
		case entities::Snake_Color_Red: {
			head_color = {1.0f, 0.4f,  0.4f,  1.0f};
			body_color = {1.0f, 0.25f, 0.25f, 1.0f};
		}break;
		case entities::Snake_Color_Green: {
			head_color = {0.5f,  0.8f, 0.5f,  1.0f};
			body_color = {0.25f, 0.8f, 0.25f, 1.0f};
		}break;
		case entities::Snake_Color_Blue: {
			head_color = {0.4f,  0.4f,  1.0f, 1.0f};
			body_color = {0.25f, 0.25f, 1.0f, 1.0f};
		}break;
		
	}
	
	f32 head_scale = 0.506f;
	f32 body_scale = 0.502f;
	
	if(!snake->is_selected_for_movement) {
		head_color.rgb *= 0.5f;
		body_color.rgb *= 0.5f;
		
		head_scale += 0.002f;
		body_scale += 0.002f;
	}


	u32 body_color32 = pack_color_argb(head_color);
	u32 head_color32 = pack_color_argb(body_color);
	f32 move_t = snake->move_t;
	
	if(snake->is_selected_for_movement) {
		draw_3d_immediate::begin();
		draw_3d_immediate::material({texture::white(), texture::black(), 1.0f});
		
		Vec3 dir = vec3(snake->move_dir);
		Vec3 prev_dir = dir;
		
		s32    seg_count = snake->segments_count;
		Vec3i* seg = snake->segments;
		Loop(i, seg_count) {
			prev_dir = dir;
			dir      = (i > 0) ? vec3(seg[i - 1] - seg[i]) : dir;
			
			u32 color = (i > 0) ? body_color32 : head_color32;
			f32 scale = (i > 0) ? body_scale : head_scale;
			
			b32 has_segment_bend_occured = (dir.x != prev_dir.x || dir.y != prev_dir.y || dir.z != prev_dir.z);
			if(has_segment_bend_occured) { 
				Vec3 pos = vec3(seg[i - 1]) + vec3(0, snake->fall_y, 0);
				draw_3d_immediate::cube(pos, vec3(scale), color);
			}
			
			{
				Vec3 pos = vec3(seg[i]) + dir*move_t + vec3(0, snake->fall_y, 0);
				draw_3d_immediate::cube(pos, vec3(scale), color);
			};
		}
		
		if(snake->should_grow) { 
			Vec3 pos = vec3(seg[seg_count - 1]) + vec3(0, snake->fall_y, 0);
			draw_3d_immediate::cube(pos, vec3(body_scale), body_color32);
		}
		
		draw_3d_immediate::end();
		return;
	}
	
	draw_3d_immediate::begin();
	draw_3d_immediate::material({texture::white(), texture::black(), 1.0f});

	Loop(i, snake->segments_count) {
		u32 color = (i > 0) ? body_color32 : head_color32;
		f32 scale = (i > 0) ? body_scale : head_scale;
		
		Vec3 pos = vec3(snake->segments[i]) + vec3(snake->move_dir)*move_t  + vec3(0, snake->fall_y, 0);
		draw_3d_immediate::cube(pos, vec3(scale), color);
	}
	draw_3d_immediate::end();
}


void entity(entities::Entity* e) {
	switch(e->type) {
		case entities::Entity_Type_Snake: { snake_draw((entities::Snake*)e); } break;
		
		case entities::Entity_Type_Crate: {
			draw_3d::material({texture::white(), texture::black(), 1.0f});
			Vec3 pos = vec3(e->segments[0]) + vec3(e->move_dir)*e->move_t + vec3(0, e->fall_y, 0);
			
			f32 s;
			Vec3 adj;
			mesh::scale_and_center(crate_mesh, 0.5f, &s, &adj);
			draw_3d::mesh(crate_mesh, pos + adj, vec3(s));
		}break;
		
		case entities::Entity_Type_Food: {
			draw_3d::material({texture::white(), texture::black(), 1.0f});
			Vec3 pos = vec3(e->segments[0]) + vec3(e->move_dir)*e->move_t + vec3(0, e->fall_y, 0);
			
			f32 s;
			Vec3 adj;
			mesh::scale_and_center(apple_mesh, 0.375f, &s, &adj);
			draw_3d::mesh(apple_mesh, pos + adj, vec3(s));
		}break;
	}
}

void level() {
	Loop(y, MAX_LEVEL_Y_DIM) {
		Loop(z, MAX_LEVEL_Z_DIM) {
			Loop(x, MAX_LEVEL_X_DIM) {
				level::Tile* tile = level::get_tile(x, y, z);
				if(level::tile_check(tile, level::Tile_Flag_Block)) {
					draw_3d::material({texture::white(), texture::black(), 1.0f});
					
					f32 s;
					Vec3 adj;
					mesh::scale_and_center(block_mesh, 0.5f, &s, &adj);
					draw_3d::mesh(block_mesh, vec3(x, y, z) + adj, vec3(s));
				}
				if(level::tile_check(tile, level::Tile_Flag_Target)) {
					draw_3d::material({texture::white(), texture::black(), 1.0f});
					
					f32 s;
					Vec3 adj;
					mesh::scale_and_center(crate_target_mesh, 0.25f, &s, &adj);
					draw_3d::mesh(crate_target_mesh, vec3(x, y, z) + adj, vec3(s));
				}
			}
		}
	}
}

} // namespace draw