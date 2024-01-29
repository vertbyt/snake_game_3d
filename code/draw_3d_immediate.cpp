
namespace draw_3d_immediate {

const s32 MAX_VERTICES = (4*4096);
const s32 MAX_INDICES  = (2*MAX_VERTICES);

struct Vertex {
	Vec3 position;
	Vec3 normal;
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
	
	Mat4 model, view, proj;
	light::Light light;
	material::Material material;
	
	shader::Shader shader;
};


Draw_State draw_state;

// ********
// Internal
void flush_vertices() {
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
	
	shader::set_mat4(shader::uloc(sh, "model"), draw_state.model.m);
	shader::set_mat4(shader::uloc(sh, "view"),  draw_state.view.m);
	shader::set_mat4(shader::uloc(sh, "proj"),  draw_state.proj.m);
	
	light::Light light = draw_state.light;
	shader::set_vec3(shader::uloc(sh, "light.dir"),      light.dir);
	shader::set_vec3(shader::uloc(sh, "light.ambient"),  light.ambient);
	shader::set_vec3(shader::uloc(sh, "light.diffuse"),  light.diffuse);
	shader::set_vec3(shader::uloc(sh, "light.specular"), light.specular);
	
	material::Material mat = draw_state.material;
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mat.diffuse.id);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, mat.specular.id);
	
	shader::set_int(shader::uloc(sh, "material.diffuse"),  0);
	shader::set_int(shader::uloc(sh, "material.specular"), 1);
	shader::set_float(shader::uloc(sh, "material.shine"),  mat.shine);
	
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

void ensure_space(s32 vertices_count, s32 indices_count) {
	b32 vertices_full = (draw_state.vertices_count + vertices_count > MAX_VERTICES);
	b32 indices_full  = (draw_state.indices_count + indices_count > MAX_INDICES);
	
	if(vertices_full || indices_full) flush_vertices();
	
	draw_state.vertices_write_at = draw_state.vertices_count;
	draw_state.indices_write_at = draw_state.indices_count;
	draw_state.indices_offset = draw_state.vertices_count;
	
	draw_state.vertices_count += vertices_count;
	draw_state.indices_count += indices_count;
}

void put_vertex(Vec3 position, Vec3 normal, Vec2 uv, u32 color) {
	Vertex* v = &draw_state.vertices[draw_state.vertices_write_at];
	*v = {position, normal, uv, color};
	
	draw_state.vertices_write_at += 1;
	Assert(draw_state.vertices_write_at <= draw_state.vertices_count);
}

void put_index(u16 index) {
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
	
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)&_Member(Vertex, normal));
	glEnableVertexAttribArray(1);
	
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)&_Member(Vertex, uv));
	glEnableVertexAttribArray(2);
	
	glVertexAttribIPointer(3, 1, GL_UNSIGNED_INT, stride, (void*)&_Member(Vertex, color));
	glEnableVertexAttribArray(3);
	
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


void mvp(Mat4 model, Mat4 view, Mat4 proj) {
	flush_vertices();
	draw_state.model = model;
	draw_state.view = view;
	draw_state.proj = proj;
}

void light(light::Light light) {
	flush_vertices();
	draw_state.light = light;
}

void material(material::Material material) {
	flush_vertices();
	draw_state.material = material;
}

void shader(shader::Shader shader) {
	flush_vertices();
	draw_state.shader = shader;
}

void vertex(Vec3 position, Vec3 normal, Vec2 uv, u32 color) {
	ensure_space(1, 1);
	put_vertex(position, normal, uv, color);
	put_index(0);
}

internal f32 cube_verts[] = {
	// front
	-1.0f,  1.0f, -1.0f,  0.0f, 0.0f, -1.0f, 0.0f, 1.0f, // top left
	-1.0f, -1.0f, -1.0f,  0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom left
	1.0f, -1.0f, -1.0f,  0.0f, 0.0f, -1.0f,  1.0f, 0.0f, // bottom right
	1.0f,  1.0f, -1.0f,  0.0f, 0.0f, -1.0f,  1.0f, 1.0f, // top right
	
	// back
	-1.0f,  1.0f, 1.0f,   0.0f, 0.0f, 1.0f,  0.0f, 1.0f, // top left
	-1.0f, -1.0f, 1.0f,   0.0f, 0.0f, 1.0f,  0.0f, 0.0f, // bottom left
	1.0f, -1.0f, 1.0f,   0.0f, 0.0f, 1.0f,   1.0f, 0.0f, // bottom right
	1.0f,  1.0f, 1.0f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f, // top right
	
	// top
	-1.0f, 1.0f,  1.0f,   0.0f, 1.0f, 0.0f,  0.0f, 1.0f, // back left
	-1.0f, 1.0f, -1.0f,   0.0f, 1.0f, 0.0f,  0.0f, 0.0f, // front left
	1.0f, 1.0f, -1.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // front right
	1.0f, 1.0f,  1.0f,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f, // back right
	
	// bottom
	-1.0f, -1.0f,  1.0f,  0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // back left
	-1.0f, -1.0f, -1.0f,  0.0f, -1.0f, 0.0f, 0.0f, 0.0f, // front left
	1.0f, -1.0f, -1.0f,  0.0f, -1.0f, 0.0f,  1.0f, 0.0f, // front right
	1.0f, -1.0f,  1.0f,  0.0f, -1.0f, 0.0f,  1.0f, 1.0f, // back right
	
	// left
	-1.0f,  1.0f,  1.0f,  -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // top back
	-1.0f, -1.0f,  1.0f,  -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom back
	-1.0f, -1.0f, -1.0f,  -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom front
	-1.0f,  1.0f, -1.0f,  -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top front
	
	// right
	1.0f,  1.0f,  1.0f,   1.0f, 0.0f, 0.0f,  0.0f, 1.0f, // top back
	1.0f, -1.0f,  1.0f,   1.0f, 0.0f, 0.0f,  0.0f, 0.0f, // bottom back
	1.0f, -1.0f, -1.0f,   1.0f, 0.0f, 0.0f,  1.0f, 0.0f, // bottom front
	1.0f,  1.0f, -1.0f,   1.0f, 0.0f, 0.0f,  1.0f, 1.0f,// top front
};

internal u32 cube_indices[] = {
	// front
	0 + 0, 0 + 1, 0 + 3,
	0 + 2, 0 + 3, 0 + 1,
	
	// back
	4 + 0, 4 + 3, 4 + 1,
	4 + 2, 4 + 1, 4 + 3,
	
	// top
	8 + 0, 8 + 1, 8 + 3,
	8 + 2, 8 + 3, 8 + 1,
	
	// bottom
	12 + 0, 12 + 3, 12 + 1,
	12 + 2, 12 + 1, 12 + 3,
	
	// left
	16 + 0, 16 + 1, 16 + 3,
	16 + 2, 16 + 3, 16 + 1,
	
	// right
	20 + 0, 20 + 3, 20 + 1,
	20 + 2, 20 + 1, 20 + 3,
};

void cube(Vec3 position, Vec3 half_size, u32 color) {
	ensure_space(6*4, 6*6);
	
	f32 sx = half_size.x;
	f32 sy = half_size.y;
	f32 sz = half_size.z;
	
	Loop(i, 24) {
		f32* v = &cube_verts[8*i];
		Vec3 p  = vec3(v[0]*sx, v[1]*sy, v[2]*sz) + position;
		Vec3 n  = {v[3], v[4], v[5]};
		Vec2 uv = {v[6], v[7]};
		
		put_vertex(p, n, uv, color);
	}
	
	Loop(i, 36) put_index(cube_indices[i]);
}


void mesh(mesh::Mesh mesh, Vec3 translation, Vec3 scale) {
	Loop(pi, mesh.partitions_count) {
		mesh::Mesh_Partition* partition = &mesh.partitions[pi];
		
		ensure_space(partition->vertices_count, partition->indices_count);
		
		mesh::Mesh_Vertex* vertices = &mesh.vertices[partition->vertex_at];
		Loop(vi, partition->vertices_count) {
			mesh::Mesh_Vertex* v = &vertices[vi];
			Vec3 p = vec3_comp_mult(v->position, scale) + translation;
			put_vertex(p, v->normal, v->uv, v->color);
		}
		
		u16* indices = &mesh.indices[partition->index_at];
		Loop(ii, partition->indices_count) put_index(indices[ii]);
	}
}

} // namespace draw_3d_immediate