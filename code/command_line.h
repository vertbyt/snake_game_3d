
#ifndef COMMAND_LINE_H
#define COMMAND_LINE_H

namespace command_line {

void init();
void update();
void render();

b32  is_enabled();
void enable();
void disable();

b32  got_command();
Str8 get_command();
void log(Str8 str);

}

#endif

