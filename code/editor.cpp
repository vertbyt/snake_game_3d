
namespace editor {

b32 should_show_command_line = false;

f32 level_world_grid_y = 0;
Camera camera;

entities::Snake_Movement the_snake_movement = entities::Snake_Movement_None;
entities::Snake_Color the_snake_color = entities::Snake_Color_None;

entities::Entity* the_entities;

Vec3 cube_edit_cursor_position;

const s32 ICON_SIZE = 48;
const s32 ICONS_PER_ROW = 5;
const s32 ICON_PAD_X = 5;
const s32 ICON_PAD_Y = 5;

enum Icon_Type {
	Icon_Type_None,
	
	Icon_Type_Block,
	Icon_Type_Target,
	Icon_Type_Crate,
	Icon_Type_Food,
	Icon_Type_Snake_Head,
	
	Icon_Type_Count
};

struct Icon {
	Vec2 position;
	texture::Texture texture;
	Icon_Type type;
};

Icon* icons;
Icon_Type icon_selected_type;

b32 got_selected_entity;
entities::Entity_Id selected_entity_id;

entities::Entity* get_entities() { return the_entities; } 

void icon_add(Icon_Type type, texture::Texture texture) {
	s32 count = fh_arr_size(icons);
	s32 y = count/ICONS_PER_ROW;
	s32 x = count - y*ICONS_PER_ROW;
	
	Icon icon;
	
	f32 xx = (f32)x*ICON_SIZE + (f32)x*ICON_PAD_X;
	f32 yy = (f32)y*ICON_SIZE + (f32)y*ICON_PAD_Y;
	icon.position = vec2(xx, yy);
	icon.texture = texture;
	icon.type = type;
	fh_arr_add(icons, icon);
}

void set_snake(entities::Snake_Color color, entities::Snake_Movement movement) {
	the_snake_color = color;
	the_snake_movement = movement;
}

void init() {	
	camera = {};
	camera.position = vec3(0, 10, -10);
	camera_set_rotation(&camera, 0.0f, 0.0f);
	
	icons = fh_arr(Icon, 32);
	icon_add(Icon_Type_Block,      block_icon_texture);
	icon_add(Icon_Type_Target,     crate_target_icon_texture);
	icon_add(Icon_Type_Crate,      crate_icon_texture);
	icon_add(Icon_Type_Food,       food_icon_texture);
	icon_add(Icon_Type_Snake_Head, snake_head_icon_texture);
	
	the_entities = fh_arr(entities::Entity, MAX_ENTITIES);
}

void camera_update() {
	f32 delta_time = window::get_delta_time();
	
	Vec3 move_dir = {};
	if(input::is_key_down(input::Key_W)) move_dir.z = 1;
	if(input::is_key_down(input::Key_S)) move_dir.z = -1;
	if(input::is_key_down(input::Key_A)) move_dir.x = -1;
	if(input::is_key_down(input::Key_D)) move_dir.x = 1;
	
	move_dir = vec3_normalize(move_dir);
	Vec3 move = camera.forward_2d*move_dir.z + camera.right_2d*move_dir.x;
	camera.position += move*delta_time*8.0f;
	
	Vec2 mouse_move_dir = vec2_normalize(vec2(input::get_relative_mouse_x(), input::get_relative_mouse_y()));
	
	f32 sensitivity = 0.035f;
	f32 yaw_delta   = mouse_move_dir.x*sensitivity;
	f32 pitch_delta = mouse_move_dir.y*sensitivity;
	
	local_persist b32 lock_rotation = false;
	if(input::is_key_pressed(input::Key_Q)) lock_rotation = !lock_rotation;
	
	if(!lock_rotation) {
		camera_update_target_rotation(&camera, yaw_delta, pitch_delta);
		camera_lerp_rotation(&camera, 0.15f);
	}
}

void update() {
	if(command_line::is_enabled()) return;
	
	camera_update();
	
	//
	// Icon selection
	//
	fh_arr_foreach(Icon, icon, icons) {
		b32 is_hovered = (Abs(icon->position.x + ICON_SIZE/2 - input::get_mouse_x()) < ICON_SIZE/2 && Abs(icon->position.y + ICON_SIZE/2 - input::get_mouse_y()) < ICON_SIZE/2);
		if(is_hovered && input::is_mouse_button_pressed(input::Mouse_Button_Left)) {
			icon_selected_type = icon->type;
		}
	}
	
	//
	// Plane positioning
	//
	f32 y_increase = 0;
	if(input::is_key_pressed(input::Key_Up))   y_increase += 1;
	if(input::is_key_pressed(input::Key_Down)) y_increase -= 1;
	
	level_world_grid_y += y_increase;
	level_world_grid_y = Clamp(level_world_grid_y, 0, MAX_LEVEL_Y_DIM - 1);
	
	//
	// Tile placement
	//
	f32 ar = (f32)window::get_width()/(f32)window::get_height();
	
	f32 rx = -1.0f + 2.0f*((f32)input::get_mouse_x()/(f32)window::get_width());
	f32 ry = -1.0f + 2.0f*((f32)input::get_mouse_y()/(f32)window::get_height());
	
	Vec4 v = vec4(rx*tanf(ToRadians(90.0f)*0.5f), (-ry*tanf(ToRadians(90.0f)*0.5f))/ar, 1.0f, 0);
	v = camera_basis(camera)*v;
	
	Vec3 cp = camera.position;
	Vec3 cr = vec3_normalize(v.xyz);
	
	level::Tile* hit_tile = NULL;
	
	{ // Plane
		f32 t = (level_world_grid_y - 0.5f - cp.y)/cr.y;
		Vec3 hit = cp + cr*t;
		
		s32 hy = (s32)level_world_grid_y;
		s32 hx = (s32)(hit.x + Sign(hit.x)*0.5f);
		s32 hz = (s32)(hit.z + Sign(hit.z)*0.5f);
		
		hit_tile = level::get_tile(vec3i(hx, hy, hz));
	};
	
	b32 place_tile = false;
	b32 delete_tile = false;
	if(hit_tile != NULL) {
		if(input::is_mouse_button_down(input::Mouse_Button_Left))  place_tile = true;
		if(input::is_mouse_button_down(input::Mouse_Button_Right)) delete_tile = true;
		
		cube_edit_cursor_position = vec3(hit_tile->pos);
	}
	
	if(place_tile && level::tile_completely_empty(hit_tile)) {
		switch(icon_selected_type) {
			case Icon_Type_Block: {
				level::block_at(hit_tile->pos);
			}break;
			case Icon_Type_Target: {
				level::target_at(hit_tile->pos);
			}break;
			case Icon_Type_Crate: {
				fh_arr_add(the_entities, entities::entity_crate(hit_tile->pos));
			}break;
			case Icon_Type_Food: {
				fh_arr_add(the_entities, entities::entity_food(hit_tile->pos));
			}break;
			case Icon_Type_Snake_Head: {
				Vec3i neighbours[] = {{1, 0, 0}, {-1, 0, 0}, {0, 0, 1}, {0, 0, -1}, {0, 1, 0}, {0, -1, 0}};
				s32 tails_count = 0;
				entities::Entity* snake = NULL;
				
				Loop(i, ArrayCount(neighbours)) {
					level::Tile* tile = level::get_tile(hit_tile->pos + neighbours[i]);
					
					if(tile == NULL) continue;
					if(!level::tile_check(tile, level::Tile_Flag_Entity)) continue;
					
					entities::Entity* e = entities::entity_find(the_entities, tile->id);
					if(e == NULL) continue;
					if(e->type != entities::Entity_Type_Snake) continue;
						
					entities::Snake* s = (entities::Snake*)e;
					if(s->color != the_snake_color) continue;
					if(s->movement != the_snake_movement) continue;
					
					b32 is_touching_last_segment = (tile->pos == e->segments[e->segments_count - 1]);
					if(is_touching_last_segment) {
						snake = e;
						break;
					}
				}
				
				if(snake == NULL) {
					s32 body = 1;
					fh_arr_add(the_entities, entities::entity_snake(the_snake_color, the_snake_movement, &body, {1,1}, hit_tile->pos));
				}
				else {
					snake->segments[snake->segments_count] = hit_tile->pos;
					snake->segments_count += 1;
				}
			}break;
		}
		
		level::clear_entities();
		level::embed_entities(the_entities);
	}else if(delete_tile) {
		if(level::tile_check(hit_tile, (level::Tile_Flag_Target|level::Tile_Flag_Block))) {
			level::empty_at(hit_tile->pos);
		}
		
		if(level::tile_check(hit_tile, level::Tile_Flag_Entity)) {
			entities::Entity* e = entities::entity_find(the_entities, hit_tile->id);
			
			switch(e->type) {
				case entities::Entity_Type_Crate:
				case entities::Entity_Type_Food: {
					e->should_remove = true;
				}break;
				case entities::Entity_Type_Snake: {
					b32 is_last_seg = (e->segments[e->segments_count - 1] == hit_tile->pos);
					b32 is_head_seg = (e->segments[0] == hit_tile->pos);
					
					if(is_head_seg) {
						Loop(i, e->segments_count - 1) e->segments[i] = e->segments[i + 1];
						e->segments_count -= 1;
					} else if(is_last_seg) {
						e->segments_count -= 1;
					}
					
					if(e->segments_count == 0) e->should_remove = true;
				}break;
			}
		}
		
		entities::remove_marked_entities(the_entities);
		level::clear_entities();
		level::embed_entities(the_entities);
	}
}

void render() {
	glViewport(0, 0, window::get_width(), window::get_height());
	
	// ****
	// 3d
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	
	glEnable(GL_MULTISAMPLE);
	
	glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	f32 ar  = (f32)window::get_width()/(f32)window::get_height();
	f32 fovx = ToRadians(90.0f);
	Mat4 proj = mat4_perspective(ar, fovx, 0.01f, 100.0f);
	Mat4 view = camera_view_matrix(camera);
	
	light::Light light = {-vec3(-10, 11.0f, -8), vec3(0.2f), vec3(1.0f), vec3(1.0f)};
	
	draw_3d::mvp(mat4_identity(), view, proj);
	draw_3d::light(light);
	draw_3d::shader(phong_shader);
	
	draw_3d_immediate::mvp(mat4_identity(), view, proj);
	draw_3d_immediate::light(light);
	draw_3d_immediate::shader(phong_shader);
	
	draw::world_grid(level_world_grid_y - 0.5f);
	
	draw::level();
	
	draw_3d_immediate::begin();
	draw_3d_immediate::material({texture::white(), texture::black(), 1.0f});
	draw_3d_immediate::cube(cube_edit_cursor_position, vec3(0.51f), 0xFF);
	draw_3d_immediate::end();

	fh_arr_foreach(entities::Entity, e, the_entities) draw::entity(e);
	
	// ******
	// 2d
	glDisable(GL_CULL_FACE);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClear(GL_DEPTH_BUFFER_BIT);
	
	draw_2d_immediate::proj(mat4_ortho(0, 0, window::get_width(), window::get_height(), 0.01f, 100.0f));
	draw_2d_immediate::shader(im_2d_shader);
	draw_2d_immediate::begin();

	level::Level* level = level::get_level();
	draw::text_line(font_small, level->name, vec2(10, window::get_height() - 10 - font_small.base_size));
	
	fh_arr_foreach(Icon, icon, icons) {
		draw_2d_immediate::texture(icon->texture);
		draw_2d_immediate::quad(vec3(icon->position, 1.0f), vec2(ICON_SIZE, ICON_SIZE), {1, 0, 0, 1}, 0xFFFFFFFF);
		
		u32 outline_color = 0xFF000000;
		
		b32 is_hovered = (Abs(icon->position.x + ICON_SIZE/2 - input::get_mouse_x()) < ICON_SIZE/2 && Abs(icon->position.y + ICON_SIZE/2 - input::get_mouse_y()) < ICON_SIZE/2);
		if(is_hovered) outline_color = 0xFFFF0000;
		
		if(icon->type == icon_selected_type) outline_color = 0xFFFFFF00;
		draw_2d_immediate::texture(icon_outline_texture);
		draw_2d_immediate::quad(vec3(icon->position, 1.0f), vec2(ICON_SIZE, ICON_SIZE), {1, 0, 0, 1}, outline_color);
	}
	
	draw_2d_immediate::end();
}

} 



