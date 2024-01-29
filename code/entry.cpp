
internal SDL_Window* sdl_window;
internal SDL_GLContext gl_context;
internal b32 running;

internal input::Game_Input game_input[2];

internal f32 delta_time;

void sdl_poll_input(void) {
	game_input[1] = game_input[0]; // store previous frame
	input::Game_Input* in = &game_input[0];
	SDL_Event event;
	while(SDL_PollEvent(&event)) {
		switch(event.type) {
			case SDL_QUIT: {
				window::quit();
			} break;
			case SDL_KEYDOWN:
			case SDL_KEYUP: {
				bool is_down = (event.key.state == SDL_PRESSED);
				SDL_Keycode key_code = event.key.keysym.sym;
				
				if(key_code >= SDLK_0 && key_code <= SDLK_9) {
					int offset = key_code - SDLK_0;
					int key_index = input::Key_0 + offset;
					in->keys[key_index] = is_down;
				}
				
				if(key_code >= SDLK_a && key_code <= SDLK_z) {
					int offset = key_code - SDLK_a;
					int key_index = input::Key_A + offset;
					in->keys[key_index] = is_down;
				}
				
				if(key_code == SDLK_SPACE)     in->keys[input::Key_Space] = is_down;
				if(key_code == SDLK_ESCAPE)    in->keys[input::Key_Escape] = is_down;
				if(key_code == SDLK_TAB)       in->keys[input::Key_Tab] = is_down;
				if(key_code == SDLK_BACKSPACE) in->keys[input::Key_Backspace] = is_down;
				if(key_code == SDLK_RETURN)    in->keys[input::Key_Enter] = is_down;

				if(key_code == SDLK_MINUS) in->keys[input::Key_Minus] = is_down;
				
				if(key_code == SDLK_BACKQUOTE) in->keys[input::Key_Tilde] = is_down;
				
				if(key_code == SDLK_UP)    in->keys[input::Key_Up]    = is_down;
				if(key_code == SDLK_DOWN)  in->keys[input::Key_Down]  = is_down;
				if(key_code == SDLK_LEFT)  in->keys[input::Key_Left]  = is_down;
				if(key_code == SDLK_RIGHT) in->keys[input::Key_Right] = is_down;
				
			}break;
			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP: {
				bool is_down = (event.button.state == SDL_PRESSED);
				u8 button = event.button.button;
				
				if(button == SDL_BUTTON_LEFT)   in->buttons[input::Mouse_Button_Left] = is_down;
				if(button == SDL_BUTTON_RIGHT)  in->buttons[input::Mouse_Button_Right] = is_down;
				if(button == SDL_BUTTON_MIDDLE) in->buttons[input::Mouse_Button_Middle] = is_down;
			}break;
		}
	}
	
	Loop(i, input::Key_Count) {
		input::Key key = (input::Key)i;
		if(input::is_key_pressed(key)) {
			in->latest_key_down = key;
			break;
		}
	}
	
	if(input::is_key_released(in->latest_key_down)) in->latest_key_down = input::Key_None;
		
	int mx, my;
	SDL_GetMouseState(&mx, &my);
	in->mouse_x = mx;
	in->mouse_y = my;
	
	SDL_GetRelativeMouseState(&mx, &my);
	in->relative_mouse_x = mx;
	in->relative_mouse_y = my;
}


void game_memory_init() {
	M_Arena* transient = game_memory::get_transient_arena();
	*transient = m_arena((u8*)malloc(TRANSIENT_ARENA_SIZE), TRANSIENT_ARENA_SIZE);
	
	M_Arena* perm = game_memory::get_permanent_arena();
	*perm = m_arena((u8*)malloc(PERMANENT_ARENA_SIZE), PERMANENT_ARENA_SIZE);
	
	M_Allocator* allocator = game_memory::get_default_allocator();
	*allocator = allocator_create((u8*)malloc(DEFAULT_ALLOCATOR_SIZE), DEFAULT_ALLOCATOR_SIZE);
}

int SDL_main(int argc, char** argv) {
	game_memory_init();
	
	engine::init();
	
	u64 pref_freq = SDL_GetPerformanceFrequency();
	u64 start_counter = SDL_GetPerformanceCounter();
	
	while(running) {
		sdl_poll_input();
		
		engine::update();
		engine::render();
		
		SDL_GL_SwapWindow(sdl_window);
		
		u64 end_counter = SDL_GetPerformanceCounter();
		delta_time = (f32)(end_counter - start_counter)/(f32)pref_freq;
		
		start_counter = end_counter;
	}
	
	return 0;
}


// **********************
// platform provided stuff

// General stuff
void window::create(char* title, s32 width, s32 height) {
	SDL_Init(SDL_INIT_EVERYTHING);
	
	int major = 3, minor = 3;
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, major);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, minor);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	
	
#if 0
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 16);
#endif
	
	
	//u32 flags = SDL_WINDOW_SHOWN|SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE|SDL_WINDOW_MAXIMIZED;
	u32 flags = SDL_WINDOW_SHOWN|SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE;

	sdl_window  = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		width, height, flags);
	
	gl_context = SDL_GL_CreateContext(sdl_window);
	
	load_gl_procs(SDL_GL_GetProcAddress);
	
	running = true;
}

s32 window::get_width() {
	s32 w, h;
	SDL_GetWindowSize(sdl_window, &w, &h);
	return w;
}

s32 window::get_height() {
	s32 w, h;
	SDL_GetWindowSize(sdl_window, &w, &h);
	return h;
}


void window::set_vsync(b32 value) { SDL_GL_SetSwapInterval(value); }
void window::quit(void) { running = false; }
f64  window::get_delta_time(void) { return delta_time; }


// Keyboard input
input::Key input::get_latest_key_down() {
	return game_input[0].latest_key_down;
}

b32 input::is_key_down(Key key){
	b32 r = game_input[0].keys[(s32)key];
	return r;
}

b32 input::is_key_pressed(Key key){
	b32 prev = game_input[1].keys[(s32)key];
	b32 curr = game_input[0].keys[(s32)key];
	b32 r = curr && !prev;
	return r;
}


b32 input::is_key_released(Key key){
	b32 prev = game_input[1].keys[(s32)key];
	b32 curr = game_input[0].keys[(s32)key];
	b32 r = !curr && prev;
	return r;
}

b32 input::is_shift_key_down() {
	SDL_Keymod keymod = SDL_GetModState();
	return(keymod & (KMOD_LSHIFT|KMOD_RSHIFT));
}

b32 input::is_caps_lock_on() {
	SDL_Keymod keymod = SDL_GetModState();
	return(keymod & (KMOD_CAPS));
}

// Mouse
b32 input::is_mouse_button_down(Mouse_Button button){
	b32 r = game_input[0].buttons[(s32)button];
	return r;
}

b32 input::is_mouse_button_pressed(Mouse_Button button){
	b32 prev = game_input[1].buttons[(s32)button];
	b32 curr = game_input[0].buttons[(s32)button];
	b32 r = curr && !prev;
	return r;
}


b32 input::is_mouse_button_released(Mouse_Button button){
	b32 prev = game_input[1].buttons[(s32)button];
	b32 curr = game_input[0].buttons[(s32)button];
	b32 r = !curr && prev;
	return r;
}

s32 input::get_mouse_x() { return game_input[0].mouse_x; }
s32 input::get_mouse_y() { return game_input[0].mouse_y; }

s32 input::get_relative_mouse_x() { return game_input[0].relative_mouse_x; }
s32 input::get_relative_mouse_y() { return game_input[0].relative_mouse_y; }

// File IO
struct File_Handle {
	void* os_handle;
	s64 file_size;
};

File_Handle file_handle_open(Str8 path) {
	char path_cstr[4096];
	str8_copy_to_cstr(path, path_cstr, sizeof(path_cstr));
	SDL_RWops *handle = SDL_RWFromFile(path_cstr, "rb");
	if(!handle) return {};
	
	s64 file_size = SDL_RWsize(handle);
	if(file_size == 0) return {};
	
	File_Handle result = {};
	result.os_handle = (void*)handle;
	result.file_size = file_size;
	return result;
}

void file_handle_close(File_Handle handle) {
	SDL_RWclose((SDL_RWops*)handle.os_handle);
}

b32 file_handle_ok(File_Handle handle) {
	return (handle.os_handle != NULL && handle.file_size > 0);
}

b32 file_handle_read_entire(File_Handle handle, u8* buffer, s64 buffer_size) {
	if(buffer_size != handle.file_size + 1) return false;
	
	s64 read_size = SDL_RWread((SDL_RWops*)handle.os_handle, buffer, handle.file_size, 1);
	if(read_size != 1) return false;
	
	buffer[handle.file_size] = '\0';
	return true;
}

b32 file_io::file_exists(Str8 path) {
	char path_cstr[KB(1)];
	str8_copy_to_cstr(path, path_cstr, sizeof(path_cstr));

	DWORD attrib = GetFileAttributes(path_cstr);
	return (attrib != INVALID_FILE_ATTRIBUTES && !(attrib & FILE_ATTRIBUTE_DIRECTORY));
}

b32 file_io::directory_exists(Str8 path) {
	char path_cstr[KB(1)];
	str8_copy_to_cstr(path, path_cstr, sizeof(path_cstr));
	
	DWORD attrib = GetFileAttributes(path_cstr);
	return (attrib != INVALID_FILE_ATTRIBUTES && (attrib & FILE_ATTRIBUTE_DIRECTORY));
}


Str8_List file_io::get_names_in_directory(M_Arena* arena, Str8 path) {
	Str8_List list = {};

	char path_cstr[KB(2)];
	str8_copy_to_cstr(path, path_cstr, sizeof(path_cstr));	
	
	if(!file_io::directory_exists(path)) return list;
	
	path = str8_concat(arena, path, str8("*"));
	str8_copy_to_cstr(path, path_cstr, sizeof(path_cstr));
	
	WIN32_FIND_DATA file_data;
	HANDLE file_handle = FindFirstFile(path_cstr, &file_data);
	
	b32 done = (FindFirstFile(path_cstr, &file_data) == INVALID_HANDLE_VALUE);
	
	s32 path_cstr_len = 0;
	while(!done) {
		path_cstr_len = 0;
		while(file_data.cFileName[path_cstr_len] != '\0') {
			path_cstr[path_cstr_len] = file_data.cFileName[path_cstr_len];
			path_cstr_len += 1;
		}
		path_cstr[path_cstr_len] = '\0';
		
		Str8 str = str8_from_cstr(arena, path_cstr);
		Str8_List_Node* node = m_alloc_struct(arena, Str8_List_Node);
		node->str = str;
		node->next = NULL;
		
		str8_list_append(&list, node);
		
		done = (FindNextFile(file_handle, &file_data) == 0);
	}
	
	return list;
}

file_io::File file_io::load(Str8 path) {
	File_Handle handle = file_handle_open(path);
	if(!file_handle_ok(handle)) return {};
	
	s64 buffer_size = handle.file_size + 1;
	u8* buffer = (u8*)m_alloc(buffer_size);
	if(!file_handle_read_entire(handle, buffer, buffer_size)) {
		m_free(buffer);
		return {};
	}
	
	file_io::File file = {buffer, 0, handle.file_size};
	file_handle_close(handle);
	
	return file;
}

file_io::File file_io::load(M_Allocator* allocator, Str8 path) {
	File_Handle handle = file_handle_open(path);
	if(!file_handle_ok(handle)) return {};
	
	s64 buffer_size = handle.file_size + 1;
	u8* buffer = (u8*)m_alloc(allocator, buffer_size);
	if(!file_handle_read_entire(handle, buffer, buffer_size)) {
		m_free(allocator, buffer);
		return {};
	}
	
	file_io::File file = {buffer, 0, handle.file_size};
	file_handle_close(handle);
	
	return file;
}

file_io::File file_io::load(M_Arena* arena, Str8 path) {
	File_Handle handle = file_handle_open(path);
	if(!file_handle_ok(handle)) return {};
	
	s64 buffer_size = handle.file_size + 1;
	
	M_Arena_Frame frame = m_arena_start_frame(arena);
	u8* buffer = (u8*)m_alloc(arena, buffer_size);
	if(!file_handle_read_entire(handle, buffer, buffer_size)) {
		m_arena_end_frame(arena, frame);
		return {};
	}
	
	file_io::File file = {buffer, 0, handle.file_size};
	file_handle_close(handle);
	
	return file;
}

b32 file_io::ok(file_io::File* file) {
	return (file->size > 0 && file->data != NULL);
}

void file_io::free(file_io::File* file) {
	if(file->data != NULL) m_free(file->data);
	file->data = NULL;
	file->size = 0;
}


void file_io::free(M_Allocator* allocator, file_io::File* file) {
	if(file->data != NULL) m_free(allocator, file->data);
	file->data = NULL;
	file->size = 0;
}

b32 file_io::write(Str8 path, file_io::File* file) {
	char path_cstr[KB(1)];
	str8_copy_to_cstr(path, path_cstr, sizeof(path_cstr));
	SDL_RWops *handle = SDL_RWFromFile(path_cstr, "wb");
	if(!handle) return false;
	
	// @note: file->at represents how far did we write into the file and that is the size to write.
	if(SDL_RWwrite(handle, file->data, file->at, 1) != 1) return false;
	
	SDL_RWclose(handle);

	return true;
}








