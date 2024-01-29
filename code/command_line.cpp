

namespace command_line {

const s32 CONSOLE_FONT_SIZE = 20;
const char* CONSOLE_FONT    = "fonts/oxygen_mono.ttf";

const s32 CONSOLE_LINES_COUNT = 24;
const s32 CONSOLE_OUTPUT_BOX_HEIGHT = CONSOLE_FONT_SIZE*CONSOLE_LINES_COUNT;
const s32 CONSOLE_OUTPUT_BOX_COLOR  = 0xFF161616;

const s32 CONSOLE_INPUT_BOX_HEIGHT = CONSOLE_FONT_SIZE*2;
const s32 CONSOLE_INPUT_BOX_COLOR  = 0xFF222222;

const s32 MAX_INPUT_STRING_LENGTH = 2048;

const s32 MAX_LOGS_COUNT = CONSOLE_LINES_COUNT;


char input_string_buffer[MAX_INPUT_STRING_LENGTH];
s32 input_string_length = 0;

input::Key last_key_pressed = input::Key_None;
s32 key_repetition_count = 0;
f32 repeated_press_wait = 0.0f;

font::Font the_font;

M_Arena log_arena;
Str8 logs[MAX_LOGS_COUNT];
s32 logs_count;

b32 the_command_line_is_enabled = false;

void enable() { the_command_line_is_enabled = true; }
void disable() { the_command_line_is_enabled = false; }
b32  is_enabled() { return the_command_line_is_enabled; }

b32 got_the_command = false;
Str8 the_command;

b32  got_command() { return got_the_command; }
Str8 get_command() { return the_command; }

void log(Str8 str) {
	logs[logs_count] = str8_copy(&log_arena, str);
	logs_count += 1;
}

void init() {
	M_Arena* perm = game_memory::get_permanent_arena();
	the_font = font::load(perm, str8((char*)CONSOLE_FONT), CONSOLE_FONT_SIZE);
	
	log_arena = m_arena((u8*)m_alloc(perm, MB(1)), MB(1));
	logs_count = 0;
}

void do_snake_command(Str8_List token_list) {
	entities::Snake_Color color = entities::Snake_Color_None;
	entities::Snake_Movement movement = entities::Snake_Movement_None;
		
	if(token_list.size < 3) {
		log(str8("Not enough parameters..."));
		log(str8(""));
		return;
	}
	
	if(token_list.size > 3) {
		log(str8("Too many parameters..."));
		log(str8(""));
		return;
	}
	
	Str8_List_Node* token = token_list.first->next;
	while(token != NULL) {
		if     (str8_equals(token->str, str8("red")))   color = entities::Snake_Color_Red;
		else if(str8_equals(token->str, str8("green"))) color = entities::Snake_Color_Green;
		else if(str8_equals(token->str, str8("blue")))  color = entities::Snake_Color_Blue;
		else if(str8_equals(token->str, str8("xz")))    movement = entities::Snake_Movement_XZ;
		else if(str8_equals(token->str, str8("xy")))    movement = entities::Snake_Movement_XY;
		
		token = token->next;
	}
	
	b32 ok = (color != entities::Snake_Color_None && movement != entities::Snake_Movement_None);
	if(ok) {
		editor::set_snake(color, movement);
		log(str8("Successfully set snake parameters..."));
		log(str8(""));
	}else {
		log(str8("Invalid parameters..."));
		log(str8(""));
	}
}

void do_level_clear_command(Str8_List token_list) {
	if(token_list.size > 1) {
		log(str8("Too many parameters..."));
		log(str8(""));
		return;
	}
	
	level::clear();
	fh_arr_reset(editor::get_entities());
	// fh_arr_reset(game::get_entities()); @todo: holy fuck
	
	log(str8("Level cleared..."));
	log(str8(""));
}

void update() {
	got_the_command = false;
	
	if(!the_command_line_is_enabled) return;
	
	b32 should_register_key = false;
	input::Key key = input::get_latest_key_down();
	if(key == last_key_pressed) {
		f32 wait[] = {0.75f, 0.02f};
		
		s32 i = Min(key_repetition_count, ArrayCount(wait) - 1);
		repeated_press_wait += window::get_delta_time();
		if(repeated_press_wait > wait[i]) {
			key_repetition_count += 1;
			repeated_press_wait = 0.0f;
			should_register_key = true;
		}
	} else {
		repeated_press_wait = 0.0f;
		key_repetition_count = 0;
		last_key_pressed = key;
		should_register_key = true;
	}
	
	if(should_register_key) {
		char a_char = 'a';
		if(input::is_shift_key_down() || input::is_caps_lock_on()) a_char = 'A';
		char c = a_char + (char)((s32)key - (s32)input::Key_A);

		if(key >= input::Key_A && key <= input::Key_Z) {
			input_string_buffer[input_string_length] = c;
			input_string_length += 1;
			input_string_buffer[input_string_length] = '\0';
		}
		if(key == input::Key_Space) {
			input_string_buffer[input_string_length] = ' ';
			input_string_length += 1;
			input_string_buffer[input_string_length] = '\0';
		}
		if(key == input::Key_Minus) {
			input_string_buffer[input_string_length] = input::is_shift_key_down() ? '_' : '-';
			input_string_length += 1;
			input_string_buffer[input_string_length] = '\0';
		}
		if(key == input::Key_Backspace) {
			if(input_string_length > 0) input_string_length -= 1;
			input_string_buffer[input_string_length] = '\0';
		}
		
		if(key == input::Key_Enter) {
			if(input_string_length > 0){
				got_the_command = true;
				the_command = str8_from_cstr(&log_arena, input_string_buffer);
				
				if(logs_count >= MAX_LOGS_COUNT) {
					logs_count = 0;
					m_arena_reset(&log_arena);
				}
				
				logs[logs_count] = the_command;
				logs_count += 1;
				
			}
			input_string_length = 0;
			input_string_buffer[input_string_length] = '\0';
		}
	} 
	
	if(got_command()) {
		u8 temp_memory[KB(2)];
		M_Arena temp_arena = m_arena(temp_memory, sizeof(temp_memory));
		
		Str8 command = get_command();
		Str8_List token_list = str8_slice_by_whitespace(&temp_arena, command);
		
		Str8_List_Node* first = token_list.first;
		if(first != NULL) {
			if(str8_equals(first->str, str8("snake"))) {
				do_snake_command(token_list);
			}
			else if(str8_equals(first->str, str8("level_clear"))) {
				do_level_clear_command(token_list);
			}
			else if(str8_equals(first->str, str8("level_new"))) {
				if(token_list.size != 2) {
					log(str8("Wrong number of parameters..."));
					log(str8(""));
				} else {
					Str8 level_name = first->next->str;
					level::create_new(level_name);
					
					// @note: Kinda strange this is seperate from the create new and level clear. 
					// Look at adding the entity array to the level structure.
					fh_arr_reset(editor::get_entities());
					// fh_arr_reset(game::get_entities()); @todo: holy fuck
					
					char level_name_cstr[256];
					str8_copy_to_cstr(level_name, level_name_cstr, 256);
					
					log(str8_fmt(&temp_arena, "New level %s...", level_name_cstr));
					log(str8(""));
				}
			}
			else if(str8_equals(first->str, str8("level_save"))) {
				level::Level* level = level::get_level();
				if(level::save(editor::get_entities())) {
					log(str8_fmt(&temp_arena, "Saved level %s...", level->name_char_buffer));
					log(str8(""));
				}else {
					log(str8_fmt(&temp_arena, "Failed to save level %s...", level->name_char_buffer));
					log(str8(""));
				}
			}
			else if(str8_equals(first->str, str8("level_load"))) {
				if(token_list.size != 2) {
					log(str8("Wrong number of parameters..."));
					log(str8(""));
				} else {
					Str8 level_name = first->next->str;
					b32 ok = level::load(level_name, editor::get_entities());
					// game::game_level_init(); @todo: holy fuck
					
					char level_name_cstr[256];
					str8_copy_to_cstr(level_name, level_name_cstr, 256);
					if(ok) {
						log(str8_fmt(&temp_arena, "Loaded level %s...", level_name_cstr));
						log(str8(""));
					}else {
						log(str8_fmt(&temp_arena, "Failed to load level %s...", level_name_cstr));
						log(str8(""));
					}
				}
			}
			else if(str8_equals(first->str, str8("camera_pos"))) {
				log(str8("TODO..."));
				log(str8(""));
			}
			else {
				log(str8("Unknown command"));
				log(str8(""));
			}
		}
	}
}

void render() {
	if(!the_command_line_is_enabled) return;

	draw_2d_immediate::shader(im_2d_shader);
	draw_2d_immediate::begin();
	
	draw_2d_immediate::texture(texture::white());
	
	// Command logs view.
	draw_2d_immediate::quad(vec3(0, 0, 1), vec2(window::get_width(), CONSOLE_OUTPUT_BOX_HEIGHT), CONSOLE_OUTPUT_BOX_COLOR);
	
	Vec2 log_pos = vec2(0, 0);
	Loop(i, logs_count) {
		draw::text_line(the_font, logs[i], log_pos, 0xFFFFFFFF);
		log_pos.y += CONSOLE_FONT_SIZE;
	}
	
	// Input Box
	draw_2d_immediate::texture(texture::white());
	draw_2d_immediate::quad(vec3(0, CONSOLE_OUTPUT_BOX_HEIGHT, 1), vec2(window::get_width(), CONSOLE_INPUT_BOX_HEIGHT), CONSOLE_INPUT_BOX_COLOR);
	
	Vec2 line_pos = vec2(5, CONSOLE_OUTPUT_BOX_HEIGHT + CONSOLE_FONT_SIZE*0.5f);
	draw::text_line(the_font, str8(input_string_buffer), line_pos);
	
	s32 input_text_width = font::text_width(the_font, str8(input_string_buffer));
	Vec3 cursor_pos = vec3(line_pos + vec2(input_text_width, 0), 1.0f);
	Vec2 cursor_dim = vec2(0.5f, 1.0f)*CONSOLE_FONT_SIZE;
	u32 cursor_color = 0xAAAAAAAA;
	
	draw_2d_immediate::texture(texture::white());
	draw_2d_immediate::quad(cursor_pos, cursor_dim, cursor_color);

	draw_2d_immediate::end();
	
}

} // namespace command_line

