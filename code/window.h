
#ifndef WINDOW_H
#define WINDOW_H

namespace window {

void create(char* title, s32 width, s32 height);
s32 get_width();
s32 get_height();

void set_vsync(b32 value);

void quit();
f64 get_delta_time();

}


#endif 