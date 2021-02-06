/* date = September 7th 2020 11:01 pm */

#define HASH_POLY 0xedb88320L
#define CRC32_LUT

#ifndef CRC32_LUT
internal void GenerateCRC32Table();
#endif

internal u32 StringToCRC32(char *string, u32 n);
internal u32 StringToCRC32(char *string);