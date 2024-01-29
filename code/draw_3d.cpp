
namespace draw_3d {

struct Vertex {
	Vec3 position;
	Vec3 normal;
	Vec2 uv;
	u32 color;
};

struct Draw_State {
	Mat4 model, view, proj;
	light::Light light;
	material::Material material;
	
	shader::Shader shader;
};


Draw_State draw_state;

// ****
// API
void init() {
	draw_state = {};
}

void mvp(Mat4 model, Mat4 view, Mat4 proj) {
	draw_state.model = model;
	draw_state.view = view;
	draw_state.proj = proj;
}

void light(light::Light light) {
	draw_state.light = light;
}

void material(material::Material material) {
	draw_state.material = material;
}

void shader(shader::Shader shader) {
	draw_state.shader = shader;
}

void mesh(mesh::Mesh mesh, Vec3 t, Vec3 s) {
		
	glBindVertexArray(mesh.vao);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);
	
	shader::Shader sh = draw_state.shader;
	shader::bind(draw_state.shader);
	
	Mat4 model = {
		s.x, 0,   0,   t.x,
		0,   s.y, 0,   t.y,
		0,   0,   s.z, t.z,
		0,   0,   0,   1
	};
	
	shader::set_mat4(shader::uloc(sh, "model"), model.m);
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
	
	glDrawElements(GL_TRIANGLES, mesh.indices_count, GL_UNSIGNED_SHORT, NULL);
	
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

} // namespace draw3d
