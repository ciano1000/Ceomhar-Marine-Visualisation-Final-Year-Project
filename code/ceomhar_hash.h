/* date = September 7th 2020 11:01 pm */

#define HASH_POLY 0xedb88320L
#define CRC32_LUT

#ifndef CRC32_LUT
INTERNAL void GenerateCRC32Table();
#endif

struct HashNode {
    void *data;
    char *key;
}

struct HashSlot {
    HashNode *node_array;
    u32 num_nodes;
}

struct HashMap {
    u32 size;
    u32 total_num_nodes;
    HashSlot *slots;
}

INTERNAL *HashMap HashInit(u32 mapSize);
INTERNAL void HashAdd(char *key, void *data);
INTERNAL *HashNode HashGet(char *key);
INTERNAL void HashDelete(char *key);

INTERNAL u32 StringToCRC32(char *string, u32 n);
INTERNAL u32 StringToCRC32(char *string);