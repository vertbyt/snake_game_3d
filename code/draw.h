
#ifndef DRAW_H
#define DRAW_H

namespace draw {

void text_line(font::Font font, Str8 text, Vec2 pos, u32 color = 0xFFFFFFFF);
void world_grid(f32 y = 0.0f, s32 extents = 15);
void entity(entities::Entity* entity);
void level();

}

#endif

