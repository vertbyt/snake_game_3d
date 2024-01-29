
#ifndef GAME_ASSETS_H
#define GAME_ASSETS_H

texture::Texture block_texture;
texture::Texture crate_texture;
font::Font font_small;

texture::Texture icon_outline_texture;
texture::Texture snake_head_icon_texture;
texture::Texture snake_body_icon_texture;
texture::Texture crate_target_icon_texture;
texture::Texture food_icon_texture;
texture::Texture crate_icon_texture;
texture::Texture block_icon_texture;

mesh::Mesh apple_mesh;
mesh::Mesh crate_mesh;
mesh::Mesh crate_target_mesh;
mesh::Mesh block_mesh;

shader::Shader phong_shader;
shader::Shader im_2d_shader;


void game_assets_load();

#endif

