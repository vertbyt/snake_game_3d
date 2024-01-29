
#ifndef FH_ALLOC_H
#define FH_ALLOC_H

#define M_ARENA_DEFAULT_ALIGNMENT (2*sizeof(void*))
#define SizeAligned(s, a) ((a)*((s)/(a)) + (a))

// @memory_arena
struct M_Arena {
	u8* base;
	s64 size;
	s64 pos, prev_pos;
};

struct M_Arena_Frame {
	M_Arena* arena;
	s64 pos;
};


M_Arena m_arena(u8* base, s64 size);
u8*     m_arena_alloc(M_Arena* arena, s64 size, s64 alignment = M_ARENA_DEFAULT_ALIGNMENT);
void    m_arena_reset(M_Arena* arena);

M_Arena_Frame m_arena_start_frame(M_Arena* arena);
void          m_arena_end_frame(M_Arena* arena, M_Arena_Frame frame);

// @general_allocator

//M_Allocation_Header and M_Free_List_Entry get intruded by each other.
struct M_Allocation_Header {
	u64 pos, size;
	M_Allocation_Header* next;
};

struct M_Free_List_Entry {
	u64 pos, size;
	M_Free_List_Entry* next;
};

struct M_Free_List {
	M_Free_List_Entry* first;
};

struct M_Allocation_List {
	M_Allocation_Header* first;
};

struct M_Allocator {
	u8* base;
	u64 size;
	
	M_Allocation_List alloc_list;
	M_Free_List free_list;
};

M_Allocator allocator_create(u8* base, u64 size);
u8*         allocator_alloc(M_Allocator* allocator, u64 desired_size);
void        allocator_free(M_Allocator* allocator, void* ptr);

// The api
void* m_alloc(u64 size);
void* m_alloc(M_Allocator* arena, u64 size);
void* m_alloc(M_Arena* arena, u64 size);

#define m_alloc_struct(allocator, Type)       (Type*)m_alloc(allocator, sizeof(Type))
#define m_alloc_array(allocator, Type, size)  (Type*)m_alloc(allocator, sizeof(Type)*size)

void m_free(void* ptr);
void m_free(M_Allocator* allocator, void* ptr);
void m_free(M_Arena* arena);

#endif
