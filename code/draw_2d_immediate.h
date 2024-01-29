
#ifndef DRAW_2D_IMMEDIATE_H
#define DRAW_2D_IMMEDIATE_H

namespace draw_2d_immediate {

void init();

void proj(Mat4 proj);
void shader(shader::Shader shader);
void texture(texture::Texture material);

void begin();
void end();

void vertex(Vec3 position, Vec2 uv, u32 color);
void quad(Vec3 position, Vec2 dim, Vec4 sub, u32 color = 0xFFFFFFFF);
void quad(Vec3 position, Vec2 dim, u32 color = 0xFFFFFFFF);

}

#endif

