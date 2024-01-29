
namespace game {

Camera camera;

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


void init() {
	camera = {};
	camera.position = vec3(0, 10, -10);
	camera_set_rotation(&camera, 0.0f, 0.0f);
}
	
void update() {
	camera_update();
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
	
	draw::level();
	fh_arr_foreach(entities::Entity, e, editor::get_entities()) draw::entity(e);
	
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
	
	draw_2d_immediate::end();

}

}