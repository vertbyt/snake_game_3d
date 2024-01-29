
namespace file_io {

void write_s32(File* file, s32 value) {
	s32* ptr = (s32*)((u8*)file->data + file->at);
	*ptr = value;
	
	file->at += sizeof(s32);
}

s32 read_s32(File* file) {
	s32* ptr = (s32*)((u8*)file->data + file->at);
	file->at += sizeof(s32);
	
	return *ptr;
}

void write_u64(File* file, u64 value) {
	u64* ptr = (u64*)((u8*)file->data + file->at);
	*ptr = value;
	
	file->at += sizeof(u64);
}

u64 read_u64(File* file) {
	u64* ptr = (u64*)((u8*)file->data + file->at);
	file->at += sizeof(u64);
	
	return *ptr;
}

}



