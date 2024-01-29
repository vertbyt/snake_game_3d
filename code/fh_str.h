
#ifndef FH_STR_H
#define FH_STR_H

struct Str8 {
  char *data;
  s64 size;
};

struct Str8_List_Node {
  Str8 str;
  Str8_List_Node* next;
};

struct Str8_List {
  Str8_List_Node* first;
  Str8_List_Node* last;
  s32 size;
};

Str8 str8(char *cstr);
Str8 str8(char *cstr, s64 size);
#define str8_lit(cstr) str8(cstr, sizeof(cstr) - 1)

void str8_list_append(Str8_List* list, Str8_List_Node* node);

Str8 str8_slice(Str8 str, s64 low, s64 high);
s32 str8_compare(Str8 a, Str8 b);
b32 str8_equals(Str8 a, Str8 b);
b32 str8_starts_with(Str8 a, Str8 b);
b32 str8_ends_with(Str8 a, Str8 b);

Str8_List str8_slice_into_lines(M_Arena* arena, Str8 str);
Str8_List str8_slice_by_whitespace(M_Arena* arena, Str8 str);
Str8 str8_concat(M_Arena* arena, Str8 first, Str8 second);
Str8 str8_copy(M_Arena* arena, Str8 s);
Str8 str8_from_cstr(M_Arena* arena, char* cstr);
Str8 str8_fmt(M_Arena* arena, char* cstr, ...);

s32 cstr_len(char *cstr);
void str8_copy_to_cstr(Str8 str, char cstr[], s32 cstr_size);

#endif

