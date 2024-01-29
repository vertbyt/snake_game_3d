
#ifndef DRAW_3D_H
#define DRAW_3D_H

namespace draw_3d {

void init();

void mvp(Mat4 model, Mat4 view, Mat4 proj);
void light(light::Light light);
void shader(shader::Shader shader);
void material(material::Material material);

void mesh(mesh::Mesh mesh, Vec3 position, Vec3 scale);

}

#endif