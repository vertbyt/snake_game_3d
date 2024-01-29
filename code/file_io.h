
#ifndef FILE_IO_H
#define FILE_IO_H

namespace file_io {

struct File {
  void* data;
  s64 at;
  s64 size;
};

File load(Str8 path);
File load(M_Allocator* allocator, Str8 path);
File load(M_Arena* arena, Str8 path);

b32 ok(File* file);

void free(File* file);
void free(M_Allocator* allocator, File* File);

void write_s32(File* file, s32 value);
s32  read_s32(File* file);

void write_u64(File* file, u64 value);
u64  read_u64(File* file);

b32 write(Str8 name, File* file);

b32 file_exists(Str8 path);
b32 directory_exists(Str8 path);
Str8_List get_names_in_directory(M_Arena* arena, Str8 path);

}



#endif

