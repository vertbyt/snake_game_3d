
namespace entities {

u64 next_entity_id_value = 0;

Entity entity_new() {
	Entity e = {};
	e.is_active = true;
	e.id = {next_entity_id_value};
	next_entity_id_value += 1;
	return e;
}

// @note: This entity_new with the id is only used in the undo code. When we crate 
// and entity again, that had been delated because it fell to it's death.
Entity entity_new(Entity_Id id) {
	Entity e = {};
	e.is_active = true;
	e.id = id;
	return e;
}

Entity* entity_find(Entity* entity_array, Entity_Id id) {
	Entity* found = NULL;
	
	auto check = [](void* t, void* e) -> b32 {
		Entity_Id* id = (Entity_Id*)t;
		Entity* entity = (Entity*)e;
		return (entity->id.value == id->value);
	};
	
	s32 fi = fh_arr_find(entity_array, &id, check);
	if(fi >= 0) found = &entity_array[fi];
	
	return found;
}

Entity* entity_find(Entity** entity_reference_array, Entity_Id id) {
	Entity* found = NULL;
	
	auto check = [](void* target, void* element) -> b32 {
		Entity_Id* id = (Entity_Id*)target;
		Entity** e = (Entity**)element;
		return (id->value == (*e)->id.value);
	};
	
	s32 fi = fh_arr_find(entity_reference_array, &id, check);
	if(fi >= 0) found = entity_reference_array[fi];
	
	return found;
}

void entity_set_segments(Entity* e, Vec3i* seg, s32 count) {
	Assert(count < MAX_ENTITY_SEGMENTS);
	e->segments_count = count;
	Loop(i, count) e->segments[i] = seg[i];
}


void remove_marked_entities(Entity* entities) {
	for(int i = 0; i < fh_arr_size(entities);) {
		Entity* e = &entities[i];
		if(e->should_remove) fh_arr_remove_at(entities, i);
		else                 i += 1;
	}
}

Entity entity_food(Vec3i pos) {
	Entity e = entity_new();
	Food* food = (Food*)&e;
	food->type = Entity_Type_Food;
	food->food_type = Food_Type_Apple;
	entity_set_segments((Entity*)food, &pos, 1);
	
	return e;
}

Entity entity_crate(Vec3i pos) {
	Entity crate = entity_new();
	crate.type = Entity_Type_Crate;
	entity_set_segments(&crate, &pos, 1);
	
	return crate;
}

Entity entity_snake(Snake_Color color, Snake_Movement movement, s32* body, Vec2i dim, Vec3i pos) {
	Entity e = entity_new();
	Snake* snake = (Snake*)&e;
	snake->type = Entity_Type_Snake;
	snake->color = color;
	snake->movement = movement;
	
	Vec3i seg[MAX_ENTITY_SEGMENTS];
	s32 seg_count = 0;
	Loop(y, dim.y) {
		Loop(x, dim.x) {
			s32 i = body[y*dim.x + x] - 1;
			if(i < 0) continue;
			
			seg[i] = pos + vec3i(x, 0, -y);
			seg_count = Max(i + 1, seg_count);
		}
	}
	
	entity_set_segments((Entity*)snake, seg, seg_count);
	
	return e;
}


} // namespace entities
