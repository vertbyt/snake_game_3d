

#ifndef GAME_MEMORY_H
#define GAME_MEMORY_H

#define TRANSIENT_ARENA_SIZE    MB(64)
#define PERMANENT_ARENA_SIZE    MB(128)
#define DEFAULT_ALLOCATOR_SIZE  MB(256)

namespace game_memory {

M_Arena*     get_transient_arena();
M_Arena*     get_permanent_arena();
M_Allocator* get_default_allocator();

}

#endif

