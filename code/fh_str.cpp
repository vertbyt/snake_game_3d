
Str8 str8(char *cstr){
	char *p = cstr;
	while(*p != '\0') p += 1;
	
	Str8 r = {cstr, (s64)(p - cstr)};
	return r;
}

Str8 str8(char *cstr, s64 size) {
	Str8 r = {cstr, size};
	return r;
}

void str8_list_append(Str8_List* list, Str8_List_Node* node) {
	if(list->first == NULL) {
		list->first = node;
		list->last = node;
		list->size = 1;
	} else {
		list->last->next = node;
		list->last = node;
		list->size += 1;
	}
}


Str8 str8_slice(Str8 str, s64 low, s64 high) {
	Str8 r = {};
	r.data = str.data + low;
	r.size = high - low;
	return r;
}

Str8_List str8_slice_into_lines(M_Arena* arena, Str8 str) {
	Str8_List list = {};
	
	s32 line_start = 0;
	s32 line_size = 0;
	
	Loop(i, str.size) {
		char c = str.data[i];
		line_size += 1;
		
		// @todo: What about \r and \r\n !!
		if(c == '\n') {
			Str8_List_Node* node = m_alloc_struct(arena, Str8_List_Node);
			node->str = str8_slice(str, line_start, line_start + line_size);
			node->next = NULL;
			
			str8_list_append(&list, node);
			
			line_start += line_size;
			line_size = 0;
		}
	}
	
	if(line_start != str.size) {
		Str8_List_Node* node = m_alloc_struct(arena, Str8_List_Node);
		node->str = str8_slice(str, line_start, str.size);
		node->next = NULL;
		str8_list_append(&list, node);
	}
	
	return list;
}

internal b32 is_whitespace(char c) {
	return(c == ' ' || c == '\n' || c == '\t');
}

Str8_List str8_slice_by_whitespace(M_Arena* arena, Str8 str) {
	Str8_List list = {};
	
	if(str.size == 0) return list;
	
	s32 token_start = 0;
	b32 in_whitespace = true;
	
	Loop(i, str.size) {
		char c = str.data[i];
		if(in_whitespace) {
			if(!is_whitespace(c)) {
				token_start = i;
				in_whitespace = false;
			}
		} else {
			if(is_whitespace(c)) {
				Str8_List_Node* node = m_alloc_struct(arena, Str8_List_Node);
				*node = {};
				node->str = str8_slice(str, token_start, i);
				str8_list_append(&list, node);
				
				in_whitespace = true;
			}
		}
	}
	
	if(!in_whitespace) {
		Str8_List_Node* node = m_alloc_struct(arena, Str8_List_Node);
		*node = {};
		node->str = str8_slice(str, token_start, str.size);
		str8_list_append(&list, node);
	}
	
	return list;
}


Str8 str8_copy(M_Arena* arena, Str8 s) {
	Str8 r = {};
	r.data = (char*)m_alloc(arena, s.size);
	r.size = s.size;
	
	Loop(i, s.size) r.data[i] = s.data[i];
	return r;
}

s32 str8_compare(Str8 a, Str8 b) {
	s32 i = 0;
	s32 size = Min(a.size, b.size);
	while(a.data[i] == b.data[i] && i <  size) i += 1;
	
	s32 r = (s32)a.data[i] - (s32)b.data[i];
	if(i == size) r = a.size - b.size;
	
	return r;
}

b32 str8_equals(Str8 a, Str8 b) {
	b32 r = (str8_compare(a, b) == 0);
	return r;
}

b32 str8_starts_with(Str8 a, Str8 b) {
	if(a.size < b.size || b.size == 0) return false;
	Loop(i, b.size) { if(a.data[i] != b.data[i]) return false; }
	return true;
}

b32 str8_ends_with(Str8 a, Str8 b) {
	if(a.size < b.size || b.size == 0) return false;
	Loop(i, b.size) { if(a.data[a.size - b.size + i] != b.data[i]) return false; }
	return true;
}

Str8 str8_concat(M_Arena* arena, Str8 first, Str8 second) {
	s32 size = first.size + second.size;
	char* data = (char*)m_arena_alloc(arena, size);
	
	char* ptr = data;
	Loop(i, first.size)  { *ptr = first.data[i]; ptr += 1; }
	Loop(i, second.size) { *ptr = second.data[i]; ptr += 1; }
	
	Str8 r = {data, size};
	return r;
}

void str8_copy_to_cstr(Str8 str, char cstr[], s32 cstr_size) {
	Assert(str.size < cstr_size);
	Loop(i, str.size) cstr[i] = str.data[i];
	cstr[str.size] = '\0';
}

Str8 str8_from_cstr(M_Arena* arena, char* cstr) {
	Str8 result = {};
	
	s32 size = cstr_len(cstr);
	result.data = (char *)m_alloc(arena, size);
	result.size = size;
	
	Loop(i, size) result.data[i] = cstr[i];
	
	return result;
}

Str8 str8_fmt(M_Arena* arena, char* cstr, ...) {
	char buff[8000] = {};
	
	va_list args;
	va_start(args, cstr);
	vsprintf(buff, cstr, args);
	va_end(args);
	
	Str8 result = str8_from_cstr(arena, buff);
	return result;
}

s32 cstr_len(char *cstr) {
	s32 r = 0;
	while(*cstr != '\0') {
		r += 1;
		cstr += 1;
	}
	return r;
}

