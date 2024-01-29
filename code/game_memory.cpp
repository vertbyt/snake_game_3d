namespace game_memory {

internal M_Arena     transient_arena;
internal M_Arena     permanent_arena;
internal M_Allocator allocator;

M_Arena*     get_transient_arena()   { return &transient_arena; }
M_Arena*     get_permanent_arena()   { return &permanent_arena; }
M_Allocator* get_default_allocator() { return &allocator; }

}

