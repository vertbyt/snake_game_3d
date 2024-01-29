
void game_assets_load() {
	M_Arena* permanent_arena = game_memory::get_permanent_arena();
	
	block_texture = texture::load(str8("block.png"));
	crate_texture = texture::load(str8("crate.png"));
	
	icon_outline_texture      = texture::load(str8("icon_outline.png"));
	snake_head_icon_texture   = texture::load(str8("snake_head_icon.png"));
	snake_body_icon_texture   = texture::load(str8("snake_body_icon.png"));
	crate_target_icon_texture = texture::load(str8("crate_target_icon.png"));
	food_icon_texture         = texture::load(str8("food_icon.png"));
	crate_icon_texture        = texture::load(str8("crate_icon.png"));
	block_icon_texture        = texture::load(str8("block_icon.png"));
	
	font_small = font::load(permanent_arena, str8_lit("fonts/liberation_mono.ttf"), 24);
	
	block_mesh        = mesh::load(str8("models/kenny/world/obj/blockRounded.obj"));
	crate_mesh        = mesh::load(str8("models/kenny/world/obj/crate.obj"));
	apple_mesh        = mesh::load(str8("models/kenny/food/obj/apple.obj"));
	crate_target_mesh = mesh::load(str8("models/kenny/world/obj/crateItem.obj"));
	
	phong_shader = shader::load(str8("../code/phong.glsl"));
	im_2d_shader = shader::load(str8("../code/im_2d.glsl"));
}