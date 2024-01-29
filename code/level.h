#ifndef LEVEL_H
#define LEVEL_H

namespace level {

enum Tile_Flag : u64 {
	Tile_Flag_Empty   = (1 << 0),
	Tile_Flag_Block   = (1 << 1),
	Tile_Flag_Entity  = (1 << 2),
	Tile_Flag_Target  = (1 << 3),
};

struct Tile {
	Vec3i pos;
	Tile_Flag flags;
	entities::Entity_Id id;
};

b32 tile_check(Tile* tile, u64 flags);
b32 tile_completely_empty(Tile* tile);
void tile_set_flag(Tile* tile, u64 flag);
void tile_add_flag(Tile* tile, u64 flag);
void tile_remove_flag(Tile* tile, u64 flag);

#define MAX_LEVEL_X_DIM 15
#define MAX_LEVEL_Y_DIM 15
#define MAX_LEVEL_Z_DIM 15

struct Level {
	Tile tiles[MAX_LEVEL_Y_DIM][MAX_LEVEL_Z_DIM][MAX_LEVEL_X_DIM];
	
	char name_char_buffer[128];
	Str8 name;
	
	s32 target_count;
	s32 food_count;
	s32 snake_count;
};

Level* get_level();

void clear();
void set_name(Str8 name);
void create_new(Str8 name);
b32 load(Str8 name, entities::Entity* entities);
b32 save(entities::Entity* entities);

Tile* get_tile(s32 x, s32 y, s32 z);
Tile* get_tile(Vec3i v);

void block_at(s32 x, s32 y, s32 z);
void block_at(Vec3i v);

void empty_at(s32 x, s32 y, s32 z);
void empty_at(Vec3i v);

void target_at(Vec3i v);

void entity_at(entities::Entity* entity, Vec3i pos);

void clear_entities();
void embed_entities(entities::Entity* entities);

void count_certain_objects(entities::Entity* entities);

}

#endif

