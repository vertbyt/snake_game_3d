

namespace level {

// **********
// Level tile
b32 tile_check(Tile* tile, u64 flags) {
	b32 r = ((tile->flags & flags) > 0);
	return r;
}

void tile_set_flag(Tile* tile, u64 flag) {
	tile->flags = (Tile_Flag)flag;
}

void tile_add_flag(Tile* tile, u64 flag) {
	tile->flags = (Tile_Flag)(tile->flags | flag);
}

void tile_remove_flag(Tile* tile, u64 flag) {
	tile->flags = (Tile_Flag)(tile->flags & (~flag));
}

b32 tile_completely_empty(Tile* tile) {
	return(((u64)tile->flags & ~((u64)Tile_Flag_Empty)) == 0);
}

// ***********
// Level
internal Level the_level;

Level* get_level() {
	return &the_level;
}

Tile* get_tile(s32 x, s32 y, s32 z) {
	if(x < 0 || y < 0 || z < 0) return NULL;
	if(x >= MAX_LEVEL_X_DIM || y >= MAX_LEVEL_Y_DIM || z >= MAX_LEVEL_Z_DIM) return NULL;
	return &the_level.tiles[y][z][x];
}

Tile* get_tile(Vec3i v) { return get_tile(v.x, v.y, v.z); }

void block_at(s32 x, s32 y, s32 z) {
	Tile* tile = get_tile(x, y, z);
	tile_set_flag(tile, Tile_Flag_Block);
}

void block_at(Vec3i v) { block_at(v.x, v.y, v.z); }

void empty_at(s32 x, s32 y, s32 z) {
	Tile* tile = get_tile(x, y, z);
	tile_set_flag(tile, Tile_Flag_Empty);
}

void empty_at(Vec3i v) { empty_at(v.x, v.y, v.z); }

void target_at(Vec3i v) {
	Tile* tile = get_tile(v);
	tile_add_flag(tile, Tile_Flag_Target);
}


void set_name(Str8 name) {
	str8_copy_to_cstr(name, the_level.name_char_buffer, sizeof(the_level.name_char_buffer));
	the_level.name = str8(the_level.name_char_buffer);
}

void clear() {
	Loop(y, MAX_LEVEL_Y_DIM) {
		Loop(z, MAX_LEVEL_Z_DIM) {
			Loop(x, MAX_LEVEL_X_DIM){
				the_level.tiles[y][z][x].pos = vec3i(x, y, z);
			}
		}
	}
	
	Loop(y, MAX_LEVEL_Y_DIM) Loop(z, MAX_LEVEL_Z_DIM) Loop(x, MAX_LEVEL_X_DIM) empty_at(x, y, z);
}

void create_new(Str8 name) {
	clear();
	set_name(name);
}

#define LEVEL_MAGIC 0x73437Fffffffff

b32 load(Str8 name, entities::Entity* entities) {	
	M_Arena* arena = game_memory::get_transient_arena();
	M_Arena_Frame arena_frame = m_arena_start_frame(arena);
	
	Str8 file_name = str8_concat(arena, name, str8(".lvl"));
	file_name = str8_concat(arena, str8("levels/"), file_name);
	file_io::File file = file_io::load(file_name);
	
	if(!file_io::ok(&file)) { 
		file_io::free(&file);
		return false;
	}
	
	u64 magic = file_io::read_u64(&file);
	if(magic != LEVEL_MAGIC) {
		file_io::free(&file);
		return false;
	}
	
	clear();
	set_name(name);
	
	s32 tiles_count = file_io::read_s32(&file);
	Loop(i, tiles_count) {
		s32 x = file_io::read_s32(&file);
		s32 y = file_io::read_s32(&file);
		s32 z = file_io::read_s32(&file);
		Tile* tile = get_tile(x, y, z);
		tile->flags = (Tile_Flag)file_io::read_u64(&file);
	}
	
	fh_arr_reset(entities);
	s32 entities_count = file_io::read_s32(&file);	
	Loop(i, entities_count) {		
		fh_arr_add(entities, entities::entity_new());
		entities::Entity* e = &entities[fh_arr_size(entities) - 1];
		
		e->type = (entities::Entity_Type)file_io::read_u64(&file);
		e->segments_count = file_io::read_s32(&file);
		Loop(i, e->segments_count) {
			s32 x = file_io::read_s32(&file);
			s32 y = file_io::read_s32(&file);
			s32 z = file_io::read_s32(&file);
			e->segments[i] = {x, y, z};
		}
		
		if(e->type == entities::Entity_Type_Snake) {
			entities::Snake* s = (entities::Snake*)e;
			s->color    = (entities::Snake_Color)file_io::read_s32(&file);
			s->movement = (entities::Snake_Movement)file_io::read_s32(&file);
		}
	}
	
	clear_entities();
	embed_entities(entities);
	
	return true;
}

b32 save(entities::Entity* entities) {
	M_Arena* arena = game_memory::get_transient_arena();
	M_Arena_Frame arena_frame = m_arena_start_frame(arena);
	
	file_io::File file = {m_alloc(arena, MB(8)), 0, MB(8)};
	
	// Get tiles
	Tile** tiles = fh_dynamic_arr(Tile*, 256);
	Loop(y, MAX_LEVEL_Y_DIM) {
		Loop(z, MAX_LEVEL_Z_DIM) {
			Loop(x, MAX_LEVEL_X_DIM){
				Tile* tile = get_tile(x, y, z);
				if(!tile_completely_empty(tile)) fh_arr_add(tiles, tile);
			}
		}
	}
	
	// Write magic
	file_io::write_u64(&file, LEVEL_MAGIC);
	
	// Write tiles
	file_io::write_s32(&file, fh_arr_size(tiles));
	fh_arr_foreach(Tile*, t, tiles) {
		Tile* tile = *t;
		file_io::write_s32(&file, tile->pos.x);
		file_io::write_s32(&file, tile->pos.y);
		file_io::write_s32(&file, tile->pos.z);
		file_io::write_u64(&file, tile->flags);
	}
	
	// Write entities
	file_io::write_s32(&file, fh_arr_size(entities));
	fh_arr_foreach(entities::Entity, e, entities) {
		file_io::write_u64(&file, e->type);
		
		file_io::write_s32(&file, e->segments_count);
		Loop(i, e->segments_count) {
			Vec3i seg = e->segments[i];
			file_io::write_s32(&file, seg.x);
			file_io::write_s32(&file, seg.y);
			file_io::write_s32(&file, seg.z);
		}
		
		if(e->type == entities::Entity_Type_Snake) {
			entities::Snake* s = (entities::Snake*)e;
			file_io::write_s32(&file, s->color);
			file_io::write_s32(&file, s->movement);
		}
	}
	
	
	Str8 file_name = str8_concat(arena, the_level.name, str8(".lvl"));
	file_name = str8_concat(arena, str8("levels/"), file_name);
	b32 result = file_io::write(file_name, &file);
	
	fh_arr_free(tiles);
	m_arena_end_frame(arena, arena_frame);
	
	return result;
}

void entity_at(entities::Entity* entity, Vec3i pos) {
	Tile* tile = get_tile(pos);
	if(tile) {
		tile_add_flag(tile, Tile_Flag_Entity);
		tile->id = entity->id;
	}
}

void clear_entities() {
	Loop(y, MAX_LEVEL_Y_DIM) {
		Loop(z, MAX_LEVEL_Z_DIM) {
			Loop(x, MAX_LEVEL_X_DIM) {
				Tile* tile = &the_level.tiles[y][z][x];
				if(tile_check(tile, Tile_Flag_Entity)) tile_remove_flag(tile, Tile_Flag_Entity);
			}
		}
	}
}

void embed_entities(entities::Entity* entities) {
	fh_arr_foreach(entities::Entity, e, entities) {
		if(!e->is_active) continue;
		
		Loop(segment_index, e->segments_count) entity_at(e, e->segments[segment_index]);
	}
}

void count_certain_objects(entities::Entity* entities) {
	the_level.target_count = 0;
	the_level.snake_count  = 0;
	the_level.food_count   = 0;
	
	Loop(y, MAX_LEVEL_Y_DIM) {
		Loop(z, MAX_LEVEL_Z_DIM) {
			Loop(x, MAX_LEVEL_X_DIM) {
				Tile* tile = &the_level.tiles[y][z][x];
				if(tile_check(tile, Tile_Flag_Target)) the_level.target_count += 1;
			}
		}
	}
	
	fh_arr_foreach(entities::Entity, e, entities) {
		if(!e->is_active) continue;
		if(e->type == entities::Entity_Type_Snake) the_level.snake_count += 1;
		if(e->type == entities::Entity_Type_Food)  the_level.food_count += 1;
	}
}

} // namespace level



