/* date = September 7th 2020 11:01 pm */

#define HASH_POLY 0xedb88320L
#define CRC32_LUT

struct String {
    char *data;
    u32 size;
};
// TODO(Cian): Push a string to some memory arena

#ifndef CRC32_LUT
internal void GenerateCRC32Table();
#endif

internal u32 StringToCRC32(char *string, u32 n);
internal u32 StringToCRC32(char *string);
internal String String_MakeString(MemoryArena *arena, char *string,...);
internal String String_AppendString(MemoryArena *arena, String *string_1, char *string_2,...);
internal String String_AppendString(MemoryArena *arena, String *string_1, String *string_2);