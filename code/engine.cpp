
namespace engine {

enum Engine_Program {
	Engine_Program_Game,
	Engine_Program_Editor,
	Engine_Program_Count
};

internal Engine_Program current_engine_program;
internal level::Level the_level_copy;

internal b32 should_show_command_line = false;

internal void game_change_to_editor() {
	current_engine_program = Engine_Program_Editor;
}

internal void editor_change_to_game() {
	current_engine_program = Engine_Program_Game;
	// game::game_level_init();
}

void init() {
	window::create("game_3d", 1280, 720);
	window::set_vsync(true);
		
	draw_3d::init();
	draw_3d_immediate::init();
	draw_2d_immediate::init();
	
	command_line::init();
	
	game_assets_load();

	editor::init();	
	game::init();

	current_engine_program = Engine_Program_Editor;
}

void update() {
	// Reset every frame.
	M_Arena* transient_arena = game_memory::get_transient_arena();
	m_arena_reset(transient_arena);
	
	if(input::is_key_pressed(input::Key_Tab)) {
		if(current_engine_program == Engine_Program_Editor) editor_change_to_game();
		else if(current_engine_program == Engine_Program_Game) game_change_to_editor();
	}
	
	if(input::is_key_pressed(input::Key_Tilde)) {
		should_show_command_line = !should_show_command_line;
		if(should_show_command_line) command_line::enable();
		else command_line::disable();
	}
	
	command_line::update();
	
	switch(current_engine_program) {
		case Engine_Program_Game:   { game::update(); } break;
		case Engine_Program_Editor: { editor::update(); } break;
	}
}

void render() {
	switch(current_engine_program) {
		case Engine_Program_Game:   { game::render(); } break;
		case Engine_Program_Editor: { editor::render(); } break;
	}
	
	command_line::render();
}


} // namespace engine

