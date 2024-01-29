
#ifndef ENTITY_H
#define ENTITY_H

#define MAX_ENTITIES 128
#define MAX_ENTITY_SEGMENTS 24

#define ENTITY_BASE \
s32   segments_count;\
Vec3i segments[MAX_ENTITY_SEGMENTS];\
\
Vec3i move_dir;\
f32 move_t;\
\
s32 fall_amount;\
f32 fall_y;\
f32 fall_time, fall_time_passed;\
b32 on_ground;\
\
b32 is_active;\
b32 should_remove;\
Entity_Id id;\
Entity_Type type\

namespace entities {
	namespace {
		enum Entity_Type : u64 {
			Entity_Type_None   = (1 << 0),
			
			Entity_Type_Entity = (1 << 1),
			Entity_Type_Snake  = (1 << 2),
			
			Entity_Type_Crate  = (1 << 3),
			Entity_Type_Food   = (1 << 4),
			
			Entity_Type_All    = ~(0)
		};
		
		struct Entity_Id {
			u64 value;
		};
		
		
		// Snake
		enum Snake_Color : s32 {
			Snake_Color_None,
			Snake_Color_Red,
			Snake_Color_Green,
			Snake_Color_Blue,
			Snake_Color_Count
		};
		
		enum Snake_Movement : s32 {
			Snake_Movement_None,
			Snake_Movement_XZ,
			Snake_Movement_XY
		};
		
		struct Snake {
			ENTITY_BASE;
			Snake_Color color;
			Snake_Movement movement;
			
			b32 should_grow;
			b32 is_selected_for_movement;
		};
		
		// Crate
		struct Crate {
			ENTITY_BASE;
		};
		
		// Food
		enum Food_Type {
			Food_Type_Apple,
			Food_Type_Pear,
			Food_Type_Orange,
			Food_Type_Cherries,
			Food_Type_Grapes,
			
			Food_Type_Count
		};
		
		struct Food {
			ENTITY_BASE;
			Food_Type food_type;
		};
		
		// "The Entity"
		struct Entity {
			union {
				struct { ENTITY_BASE; };
				
				Snake snake;
				Crate crate;
				Food  food;
			};
		};
	}
	
	Entity entity_new();
	Entity entity_new(Entity_Id id);
	
	Entity* entity_find(Entity* entity_array, Entity_Id id);
	Entity* entity_find(Entity** entity_reference_array, Entity_Id id);
	void entity_set_segments(Entity* e, Vec3i* seg, s32 count);
	
	void remove_marked_entities(Entity* entities);
	
	Entity entity_food(Vec3i pos);
	Entity entity_crate(Vec3i pos);
	Entity entity_snake(Snake_Color color, Snake_Movement movement, s32* body, Vec2i dim, Vec3i pos);

}

#endif

