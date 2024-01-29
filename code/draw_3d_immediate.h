

#ifndef DRAW_3D_IMMEDIATE_H
#define DRAW_3D_IMMEDIATE_H

namespace draw_3d_immediate {

void init();

void mvp(Mat4 model, Mat4 view, Mat4 proj);

void light(light::Light light);
void shader(shader::Shader shader);
void material(material::Material material);

// Immediate 3D
void begin();
void end();

void vertex(Vec3 position, Vec3 normal, Vec2 uv, u32 color);
void cube(Vec3 position, Vec3 half_size, u32 color = 0xFFFFFFFF);
void mesh(mesh::Mesh mesh, Vec3 position, Vec3 scale);

}

#endif

