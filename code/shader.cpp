
namespace shader {

internal Shader the_current_bound_shader = {};

// Shader creation
internal Shader compile(Str8 src, Shader_Type type) {
	Assert(type == Shader_Type_Vertex || type == Shader_Type_Fragment);
	
	s32 gl_type = 0;
	if(type == Shader_Type_Vertex)   gl_type = GL_VERTEX_SHADER;
	if(type == Shader_Type_Fragment) gl_type = GL_FRAGMENT_SHADER;
	
	u32 id = glCreateShader(gl_type);
	
	s32 size32 = (s32)src.size;
	glShaderSource(id, 1, &src.data, &size32);
	glCompileShader(id);
	
	int ok;
	char fail_info[1024];
	glGetShaderiv(id, GL_COMPILE_STATUS, &ok);
	if(!ok) {
		glGetShaderInfoLog(id, sizeof(fail_info), NULL, fail_info);
		Assert(!"Shader compilation failed!!!!");
	}
	
	Shader r = {id, type};
	return r;
}

internal Shader load_and_compile(Str8 path, Shader_Type type) {
	file_io::File file = file_io::load(path);
	Str8 file_str = {(char* )file.data, file.size};
	
	Shader r = compile(file_str, type);
	
	file_io::free(&file);
	
	return r;
}

internal Shader link(Shader shaders[Shader_Type_Count]) {
	u32 id = glCreateProgram();
	
	Loop(i, Shader_Type_Count) {
		if(shaders[i].id == 0) continue;
		glAttachShader(id, shaders[i].id);
	}
	glLinkProgram(id);
	
	int ok;
	char fail_info[1024];
	glGetProgramiv(id, GL_LINK_STATUS, &ok);
	if(!ok) {
		glGetProgramInfoLog(id, sizeof(fail_info), NULL, fail_info);
		Assert(!"Shader program linking failed!!!!");
	}
	
	Shader r = {id, Shader_Type_Program};
	return r;
}

void unload(Shader shader) {
	if(shader.type == Shader_Type_None) return;
	
	if(shader.type == Shader_Type_Program) glDeleteProgram(shader.id);
	else glDeleteShader(shader.id);
}


Shader vs_fs(Str8 vs, Str8 fs) {
	Shader shaders[Shader_Type_Count] = {};
	
	shaders[Shader_Type_Vertex]   = compile(vs, Shader_Type_Vertex);
	shaders[Shader_Type_Fragment] = compile(fs, Shader_Type_Fragment);
	
	Shader r = link(shaders);
	return r;
}

Shader load(Str8 path) {
	file_io::File file = file_io::load(path);
	Str8 file_str = {(char* )file.data, file.size};
	
	u8 mem[8192];
	M_Arena arena = m_arena(mem, sizeof(mem));
	
	Shader shaders[Shader_Type_Count] = {};
	Shader_Type type = Shader_Type_None;
	
	Str8_List lines = str8_slice_into_lines(&arena, file_str);
	Str8_List_Node* line = lines.first;
	s32 bytes_read = 0, shader_src_start_byte = 0;
	
	Loop(i, lines.size) {
		Str8 line_str = line->str;
		line = line->next;
		
		if(line_str.data[0] == '@') {
			if(type != Shader_Type_None) {
				Str8 shader_src = str8_slice(file_str, shader_src_start_byte, bytes_read);
				shaders[type] = compile(shader_src, type);
			}
			
			if(str8_starts_with(line_str, str8_lit("@vertex")))   type = Shader_Type_Vertex;
			if(str8_starts_with(line_str, str8_lit("@fragment"))) type = Shader_Type_Fragment;
			
			Str8_List tokens = str8_slice_by_whitespace(&arena, line_str);
			if(tokens.size > 1) {
				shaders[type] = load_and_compile(tokens.first->next->str, type);
				type = Shader_Type_None;
			} else {
				shader_src_start_byte = bytes_read + line_str.size;
			}
		}
		
		bytes_read += line_str.size;
	}
	if(type != Shader_Type_None) {
		Str8 shader_src = str8_slice(file_str, shader_src_start_byte, bytes_read);
		shaders[type] = compile(shader_src, type);
	}
	
	file_io::free(&file);
	
	Shader r = link(shaders);
	return r;
}

// Shader binding
void bind(Shader shader) {
	if(shader.type == Shader_Type_Program) glUseProgram(shader.id);
	else                                   Assert(!"Shader type isn't Shader_Type_Program!!");
	the_current_bound_shader = shader;
}

void unbind(void) {
	glUseProgram(0);
	the_current_bound_shader = {};
}

b32 is_current_bound(Shader shader) {
	return(shader.id == the_current_bound_shader.id);
}

// Shader uniforms
s32 uloc(Shader shader, char* name)  {
	s32 loc = glGetUniformLocation(shader.id, name);
	//Assert(loc != -1);
	return loc;
}


void set_int(s32 loc, s32 value)             { glUniform1i(loc, value);  }
void set_ints(s32 loc, s32 count, s32* ints) { glUniform1iv(loc, count, ints); }

void set_float(s32 loc, f32 v)              { glUniform1f(loc, v); }
void set_floats(s32 loc, s32 count, f32* v) { glUniform1fv(loc, count, v); }


void set_mat4(s32 loc, f32* m) { glUniformMatrix4fv(loc, 1, GL_TRUE, m); }

void set_vec3(s32 loc, f32 x, f32 y, f32 z) { glUniform3f(loc, x, y, z);       }
void set_vec3(s32 loc, Vec3 v)              { glUniform3f(loc, v.x, v.y, v.z); }

void set_vec4(s32 loc, f32 x, f32 y, f32 z, f32 w) { glUniform4f(loc, x, y, z, w); }


} // namespace shader

