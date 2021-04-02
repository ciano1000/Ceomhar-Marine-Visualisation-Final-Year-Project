// NOTE(Cian): Pre-Computed: All possible 8 bit CRC values for polynomial 0xedb88320
#pragma warning(push)
#pragma warning(disable: 4505)

global u32 CRC32_Table[256] = {
    0x00000000,0x77073096,0xee0e612c,0x990951ba,0x076dc419,0x706af48f,0xe963a535,0x9e6495a3,                                                                                             
    0x0edb8832,0x79dcb8a4,0xe0d5e91e,0x97d2d988,0x09b64c2b,0x7eb17cbd,0xe7b82d07,0x90bf1d91,                                                                                             
    0x1db71064,0x6ab020f2,0xf3b97148,0x84be41de,0x1adad47d,0x6ddde4eb,0xf4d4b551,0x83d385c7,                                                                                             
    0x136c9856,0x646ba8c0,0xfd62f97a,0x8a65c9ec,0x14015c4f,0x63066cd9,0xfa0f3d63,0x8d080df5,                                                                                             
    0x3b6e20c8,0x4c69105e,0xd56041e4,0xa2677172,0x3c03e4d1,0x4b04d447,0xd20d85fd,0xa50ab56b,                                                                                             
    0x35b5a8fa,0x42b2986c,0xdbbbc9d6,0xacbcf940,0x32d86ce3,0x45df5c75,0xdcd60dcf,0xabd13d59,                                                                                             
    0x26d930ac,0x51de003a,0xc8d75180,0xbfd06116,0x21b4f4b5,0x56b3c423,0xcfba9599,0xb8bda50f,                                                                                             
    0x2802b89e,0x5f058808,0xc60cd9b2,0xb10be924,0x2f6f7c87,0x58684c11,0xc1611dab,0xb6662d3d,                                                                                             
    0x76dc4190,0x01db7106,0x98d220bc,0xefd5102a,0x71b18589,0x06b6b51f,0x9fbfe4a5,0xe8b8d433,                                                                                             
    0x7807c9a2,0x0f00f934,0x9609a88e,0xe10e9818,0x7f6a0dbb,0x086d3d2d,0x91646c97,0xe6635c01,                                                                                             
    0x6b6b51f4,0x1c6c6162,0x856530d8,0xf262004e,0x6c0695ed,0x1b01a57b,0x8208f4c1,0xf50fc457,                                                                                             
    0x65b0d9c6,0x12b7e950,0x8bbeb8ea,0xfcb9887c,0x62dd1ddf,0x15da2d49,0x8cd37cf3,0xfbd44c65,                                                                                             
    0x4db26158,0x3ab551ce,0xa3bc0074,0xd4bb30e2,0x4adfa541,0x3dd895d7,0xa4d1c46d,0xd3d6f4fb,                                                                                             
    0x4369e96a,0x346ed9fc,0xad678846,0xda60b8d0,0x44042d73,0x33031de5,0xaa0a4c5f,0xdd0d7cc9,                                                                                             
    0x5005713c,0x270241aa,0xbe0b1010,0xc90c2086,0x5768b525,0x206f85b3,0xb966d409,0xce61e49f,                                                                                             
    0x5edef90e,0x29d9c998,0xb0d09822,0xc7d7a8b4,0x59b33d17,0x2eb40d81,0xb7bd5c3b,0xc0ba6cad,                                                                                             
    0xedb88320,0x9abfb3b6,0x03b6e20c,0x74b1d29a,0xead54739,0x9dd277af,0x04db2615,0x73dc1683,                                                                                             
    0xe3630b12,0x94643b84,0x0d6d6a3e,0x7a6a5aa8,0xe40ecf0b,0x9309ff9d,0x0a00ae27,0x7d079eb1,                                                                                             
    0xf00f9344,0x8708a3d2,0x1e01f268,0x6906c2fe,0xf762575d,0x806567cb,0x196c3671,0x6e6b06e7,                                                                                             
    0xfed41b76,0x89d32be0,0x10da7a5a,0x67dd4acc,0xf9b9df6f,0x8ebeeff9,0x17b7be43,0x60b08ed5,                                                                                             
    0xd6d6a3e8,0xa1d1937e,0x38d8c2c4,0x4fdff252,0xd1bb67f1,0xa6bc5767,0x3fb506dd,0x48b2364b,                                                                                             
    0xd80d2bda,0xaf0a1b4c,0x36034af6,0x41047a60,0xdf60efc3,0xa867df55,0x316e8eef,0x4669be79,                                                                                             
    0xcb61b38c,0xbc66831a,0x256fd2a0,0x5268e236,0xcc0c7795,0xbb0b4703,0x220216b9,0x5505262f,                                                                                             
    0xc5ba3bbe,0xb2bd0b28,0x2bb45a92,0x5cb36a04,0xc2d7ffa7,0xb5d0cf31,0x2cd99e8b,0x5bdeae1d,                                                                                             
    0x9b64c2b0,0xec63f226,0x756aa39c,0x026d930a,0x9c0906a9,0xeb0e363f,0x72076785,0x05005713,                                                                                             
    0x95bf4a82,0xe2b87a14,0x7bb12bae,0x0cb61b38,0x92d28e9b,0xe5d5be0d,0x7cdcefb7,0x0bdbdf21,                                                                                             
    0x86d3d2d4,0xf1d4e242,0x68ddb3f8,0x1fda836e,0x81be16cd,0xf6b9265b,0x6fb077e1,0x18b74777,                                                                                             
    0x88085ae6,0xff0f6a70,0x66063bca,0x11010b5c,0x8f659eff,0xf862ae69,0x616bffd3,0x166ccf45,                                                                                             
    0xa00ae278,0xd70dd2ee,0x4e048354,0x3903b3c2,0xa7672661,0xd06016f7,0x4969474d,0x3e6e77db,                                                                                             
    0xaed16a4a,0xd9d65adc,0x40df0b66,0x37d83bf0,0xa9bcae53,0xdebb9ec5,0x47b2cf7f,0x30b5ffe9,                                                                                             
    0xbdbdf21c,0xcabac28a,0x53b39330,0x24b4a3a6,0xbad03605,0xcdd70693,0x54de5729,0x23d967bf,                                                                                             
    0xb3667a2e,0xc4614ab8,0x5d681b02,0x2a6f2b94,0xb40bbe37,0xc30c8ea1,0x5a05df1b,0x2d02ef8d,  
};

#ifndef CRC32_LUT
internal void string_generate_crc32_table() {
    
    u32 n, crc;
    for(n = 0; n < 256; ++n) {
        crc = n;
        // NOTE(Cian): Loop through each bit
        for(u32 k = 0; k < 8; ++k) {
            // NOTE(Cian): If the MSB is 1
            if(crc & 1) {
                crc = HASH_POLY ^ (c >> 1);
            } else {
                crc = c >> 1;
            }
        }
        CRC32_Table[n] = c;
    }
}
#endif


internal u32 string_to_crc32(char *string, u32 n, u32 seed) {
#ifndef CRC32_LUT
    generate_crc32_table();
#endif
    u32 crc = seed;
    for(u32 i = 0; i < n; ++i) {
        u32 pos = (crc ^ string[i]) & 255;
        crc =(CRC32_Table[pos] ^ (crc >> 8));
    }
    return crc;
}

internal u32 string_to_crc32(char *string, u32 seed) {
    u32 crc = string_to_crc32(string, (u32)strlen(string), seed);
    return crc;
}

internal String8 string_from_cstring(char *string) {
    String8 result = {};
    result.data = string;
    
    u32 size = 0;
    if(string) {
        
        
        char *curr = string;
        while((*curr) != null) {
            size++;
            curr += 1;
        }
        size++;
        
        result.size = size;
    }
    return result;
}

internal String8 string_push(Memory_Arena *arena, u32 size) {
    String8 result = {};
    result.size = size;
    
    result.data = (char*)memory_arena_push(arena, size);
    
    result.data[size - 1] = null;
    
    return result;
}

internal String8 string_make(Memory_Arena *arena, char *string,...) {
    String8 result = {};
    
    va_list args;
    va_start(args, string);
    u32 size = stbsp_vsnprintf(null, 0, string, args) + 1;
    va_end(args);
    
    result.data = (char*)memory_arena_push(arena, size);
    // TODO(Cian): Have error logging to check if memory allocation was successful or not
    if(result.data) {
        result.size = size;
        
        va_start(args, string);
        stbsp_vsnprintf(result.data, size, string, args);
        va_end(args);
        
        result.data[size - 1] = null;
    }
    
    return result;
}

internal String8 string_make(Memory_Arena *arena, char *string, va_list args) {
    String8 result = {};
    
    u32 size = stbsp_vsnprintf(null, 0, string, args) + 1;
    
    result.data = (char*)memory_arena_push(arena, size);
    // TODO(Cian): Have error logging to check if memory allocation was successful or not
    if(result.data) {
        result.size = size;
        stbsp_vsnprintf(result.data, size, string, args);
        result.data[size - 1] = null;
    }
    
    return result;
}

internal String8 string_append(Memory_Arena *arena, String8 *string_1, String8 *string_2) {
    String8 result = {};
    
    u32 size = (string_1->size + string_2->size) - 1;
    result.data = (char*)memory_arena_push(arena, size);
    result.size = size;
    
    if(result.data) {
        stbsp_snprintf(result.data, string_1->size, string_1->data);
        stbsp_snprintf(result.data + (string_1->size - 1), string_2->size, string_2->data);
        result.data[size - 1] = null;
    }
    
    return result;
}

// TODO(Cian): Maybe this shouldn't return a new string and should just reallocate the original?? Idk, revaluate after using it for a bit
internal String8 string_append(Memory_Arena *arena, String8 string_1, char *string_2, ...) {
    // TODO(Cian): Seem to be redoing a lot of stuff here?
    String8 result = {};
    
    va_list args;
    va_start(args, string_2);
    // NOTE(Cian): String8.size already includes null terminator so don't need to +1 vsnprintf result
    u32 str_2_size = string_1.size + (stbsp_vsnprintf(null, 0, string_2, args));
    u32 size = string_1.size + str_2_size;
    va_end(args);
    
    result.data = (char*)memory_arena_push(arena, size);
    
    if(result.data) {
        stbsp_snprintf(result.data, string_1.size, string_1.data);
        va_start(args, string_2);
        stbsp_vsnprintf(result.data + (string_1.size - 1), size, string_2, args);
        va_end(args);
        
    }
    
    return result;
}

internal void string_copy(String8 dest, String8 src, u32 src_offset, u32 count) {
    assert(count <= dest.size); 
    assert((src_offset + (count - 1)) < src.size);
    
    for(u32 i = 0; i < count - 1; i++) {
        dest.data[i] = src.data[src_offset + i];
    }
    dest.data[count - 1] = null;
}

internal b32 char_is_digit(char c) {
    return c >= '0' && c <= '9';
}

internal b32 char_is_letter(char c) {
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

internal char char_to_upper(char c) {
    return (c >= 'a' && c <= 'z') ? c - 32 : c;
}

internal char char_to_lower(char c) {
    return (c >= 'A' && c <= 'Z') ? c + 32 : c;
}

internal void string_to_upper(String8 string) {
    for(u32 i = 0; i < string.size; i++) {
        string.data[i] = char_to_upper(string.data[i]);
    }
}

internal void string_to_lower(String8 string) {
    for(u32 i = 0; i < string.size; i++) {
        string.data[i] = char_to_lower(string.data[i]);
    }
}

internal void string_gen_random(char *s, int len) {
    static const char alphanum[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    
    for (int i = 0; i < len -1; ++i) {
        s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
    }
    
    s[len-1] = 0;
}

internal b32 string_compare(String8 str_1, String8 str_2) {
    b32 result = false;
    
    if(str_1.size == str_2.size) {
        result = true;
        
        for(u32 i = 0; i < str_1.size; i++) {
            if(str_1.data[i] != str_2.data[i]) 
                result = false;
        }
    }
    
    return result;
}

internal String8 string_tokenizer(Memory_Arena *arena, String8 main_string, char delimiter, u32 *bytes_read) {
    u32 size = 0;
    u32 skip_count = 0;
    
    while(main_string.data[(*bytes_read) + size] != delimiter) {
        char curr = main_string.data[(*bytes_read) + size];
        
        if(curr == '\0' || curr == '\n') {
            break;
        } else if(curr == '\r') {
            char next_curr = main_string.data[(*bytes_read) + size + 1];
            if(next_curr == '\n') {
                skip_count++;
                break;
            }
        }
        size++;
    }
    size++;
    
    String8 result = {};
    result = string_push(arena, size);
    string_copy(result, main_string, *bytes_read, size);
    (*bytes_read) += size + skip_count;
    
    return result;
}

internal b32 string_is_float(String8 string) {
    b32 result = false;
    for(u32 i = 0; i < string.size - 1; i++) {
        if(string.data[i] == '.') {
            result = true;
        } else {
            if(string.data[i] < '0' || string.data[i] > '9') {
                result = false;
                break;
            }
        }
    }
    
    return result;
}

internal u32 string_to_u32(String8 decimal_string) {
    u32 result = 0;
    
    char *curr_char = decimal_string.data;
    while(*curr_char) {
        result = 10 * result + (*curr_char++ - '0');
    }
    
    return result;
}

internal f32 string_to_f32(String8 float_string) {
    
    return strtof(float_string.data, null);
}

enum HexString8Endianness{
    HexString8_LittleEndian,
    HexString8_BigEndian,
};

internal u32 string_hex_to_u32(String8 hex_string, HexString8Endianness endianness) {
    // NOTE(Cian): This function assumes we are running on a little endian machine, endianness arg refers to the endianness of the incoming hex string
    
    char *curr_char = hex_string.data;
    
    u32 result = 0;
    if(endianness == HexString8_BigEndian) {
        while(*curr_char) {
            //Each hex digit is 4 bits, so shift left 4 bits each time, this naturally handles BigEndian on our LittleEndian machine as the leftmost byte is the MSB, and is left shifted to the same position in memory
            result = (result << 4) | hextable[*curr_char++];
        }
    } else {
        curr_char = hex_string.data + (hex_string.size - 2);
        
        for(u32 i = 1; i < hex_string.size; i++) {
            //some "fancy" flipping between -1: when i is odd & 1: when i is even, this is to ensure bytes are read in correct ordering
            s32 offset = ((((i % 2) - 1) * -1) * 2) - 1;
            
            result = (result << 4) | hextable[*(curr_char + offset)];
            curr_char--;
        }
        
    }
    return result;
}
#pragma warning(pop)