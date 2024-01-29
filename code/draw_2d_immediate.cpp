
namespace draw_2d_immediate {


const s32 MAX_VERTICES = (2*4096);
const s32 MAX_INDICES  = (2*MAX_VERTICES);

struct Vertex {
	Vec3 position;
	Vec2 uv;
	u32 color;
};

struct Draw_State {
	u32 vao, vbo, ebo;
	
	Vertex* vertices;
	u16* indices;
	
	u32 vertices_count;
	u32 indices_count;
	
	u32 vertices_write_at;
	u32 indices_write_at;
	u32 indices_offset;
	
	Mat4 proj;
	texture::Texture texture;
	shader::Shader shader;
};


internal Draw_State draw_state;

// ********
// Internal
internal void flush_vertices() {
	if(draw_state.vertices_count == 0) return;

	glBindVertexArray(draw_state.vao);
	
	glBindBuffer(GL_ARRAY_BUFFER, draw_state.vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, draw_state.ebo);

	u32 vbo_size = sizeof(Vertex)*draw_state.vertices_count;
	u32 ebo_size = sizeof(u16)*draw_state.indices_count;
	
	glBufferSubData(GL_ARRAY_BUFFER, 0, vbo_size, draw_state.vertices);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, ebo_size, draw_state.indices);
	
	shader::Shader sh = draw_state.shader;
	shader::bind(draw_state.shader);
	
	shader::set_mat4(shader::uloc(sh, "proj"),  draw_state.proj.m);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, draw_state.texture.id);
	
	shader::set_int(shader::uloc(sh, "the_texture"),  0);
	
	glDrawElements(GL_TRIANGLES, draw_state.indices_count, GL_UNSIGNED_SHORT, NULL);
	
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	draw_state.vertices_count = 0;
	draw_state.indices_count = 0;
	
	draw_state.vertices_write_at = 0;
	draw_state.indices_write_at = 0;
	draw_state.indices_offset = 0;
}

internal void draw_flush() {
	flush_vertices();
}

internal void ensure_space(s32 vertices_count, s32 indices_count) {
	b32 vertices_full = (draw_state.vertices_count + vertices_count > MAX_VERTICES);
	b32 indices_full  = (draw_state.indices_count + indices_count > MAX_INDICES);
	
	if(vertices_full || indices_full) flush_vertices();
	
	draw_state.vertices_write_at = draw_state.vertices_count;
	draw_state.indices_write_at = draw_state.indices_count;
	draw_state.indices_offset = draw_state.vertices_count;
	
	draw_state.vertices_count += vertices_count;
	draw_state.indices_count += indices_count;
}

internal void put_vertex(Vec3 position, Vec2 uv, u32 color) {
	Vertex* v = &draw_state.vertices[draw_state.vertices_write_at];
	*v = {position, uv, color};
	
	draw_state.vertices_write_at += 1;
	Assert(draw_state.vertices_write_at <= draw_state.vertices_count);
}

internal void put_index(u16 index) {
	draw_state.indices[draw_state.indices_write_at] = draw_state.indices_offset + index;
	
	draw_state.indices_write_at += 1;
	Assert(draw_state.indices_write_at <= draw_state.indices_count);
}


// ****
// API
void init() {
	M_Arena* arena = game_memory::get_permanent_arena();
	
	u32 vertices_size = MAX_VERTICES*sizeof(Vertex);
	u32 indices_size = MAX_INDICES*sizeof(u16);
	
	Vertex* vertices = (Vertex *)m_alloc(arena, vertices_size);
	u16*    indices  = (u16 *)m_alloc(arena, indices_size);
	
	// buffers
	u32 vao, vbo, ebo;
	
	// gen
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);
	
	// data
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	
	glBufferData(GL_ARRAY_BUFFER, vertices_size, NULL, GL_DYNAMIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_size, NULL, GL_DYNAMIC_DRAW);
	
	// vertex attrib
	u32 stride = sizeof(Vertex);
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)&_Member(Vertex, position));
	glEnableVertexAttribArray(0);
	
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)&_Member(Vertex, uv));
	glEnableVertexAttribArray(1);
	
	glVertexAttribIPointer(2, 1, GL_UNSIGNED_INT, stride, (void*)&_Member(Vertex, color));
	glEnableVertexAttribArray(2);
	
	// unbind
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	
	// set draw state
	draw_state = {};
	
	draw_state.vao = vao;
	draw_state.vbo = vbo;
	draw_state.ebo = ebo;
	
	draw_state.vertices = vertices;
	draw_state.indices = indices;
}

void begin() {
	flush_vertices();
}

void end() {
	flush_vertices();
}


void proj(Mat4 proj) {
	draw_flush();
	draw_state.proj = proj;
}

void shader(shader::Shader shader) {
	draw_flush();
	draw_state.shader = shader;
}

void texture(texture::Texture texture) {
	draw_flush();
	draw_state.texture = texture;
}


void vertex(Vec3 position, Vec2 uv, u32 color) {
	ensure_space(1, 1);
	put_vertex(position, uv, color);
	put_index(0);
}

void quad(Vec3 position, Vec2 dim, Vec4 sub, u32 color) {
	ensure_space(4, 6);
	
	Vec3 p[4] = {
		position,
		position + vec3(0.0f,  dim.y, 0.0f),
		position + vec3(dim.x, dim.y, 0.0f),
		position + vec3(dim.x, 0.0f, 0.0f)
	};
	
	Vec2 uv[4] = {
		{sub.left, sub.top},
		{sub.left, sub.bottom},
		{sub.right, sub.bottom},
		{sub.right, sub.top}
	};
	
	put_vertex(p[0], uv[0], color);
	put_vertex(p[1], uv[1], color);
	put_vertex(p[2], uv[2], color);
	put_vertex(p[3], uv[3], color);
	
	u16 indices[6] = {0, 1, 3, 2, 3, 1};
	Loop(i, 6) put_index(indices[i]);
}

void quad(Vec3 position, Vec2 dim, u32 color) {
	quad(position, dim, {1, 0, 0, 1}, color);
}

} // namespace draw_2d_immediate 